#include "depthanalyzer.h"

#include <s3d/cv/features/match_finder_cv.h>
#include <s3d/cv/utilities/cv.h>
#include <s3d/disparity/utilities.h>
#include <s3d/multiview/sampson_distance_function.h>
#include <s3d/multiview/stan_fundamental_matrix_solver.h>
#include <s3d/robust_estimation/ransac.h>
#include <s3d/utilities/histogram.h>

#include <opencv2/opencv.hpp>

// todo: clean this

namespace {
void displayInNewWindow(const std::string& name, cv::InputArray src) {
  cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
  cv::imshow(name, src);
  cv::waitKey(0);
}

void drawFeatures(cv::Mat img, const std::vector<Eigen::Vector3d>& pts) {
  for (auto& pt : pts) {
    cv::circle(img, cv::Point(pt.x(), pt.y()), 2, cv::Scalar(255, 0, 0, 0), -1);
  }
}

void displayMatches(const std::string& displayName,
                    cv::Mat imgLeft,
                    cv::Mat imgRight,
                    std::vector<Eigen::Vector3d> ptsLeft,
                    std::vector<Eigen::Vector3d> ptsRight) {
  assert(ptsLeft.size() == ptsRight.size());
  assert(imgLeft.size == imgRight.size);

  drawFeatures(imgLeft, ptsLeft);
  drawFeatures(imgRight, ptsRight);

  cv::Mat combined(imgLeft.rows, imgLeft.cols * 2, imgLeft.type());
  cv::hconcat(imgLeft, imgRight, combined);

  for (int i = 0; i < ptsLeft.size(); ++i) {
    auto& pt1 = ptsLeft[i];
    Eigen::Vector3d pt2 = ptsRight[i] + Eigen::Vector3d(imgLeft.cols, 0.0, 0.0);
    cv::line(combined, cv::Point(pt1.x(), pt1.y()), cv::Point(pt2.x(), pt2.y()),
             cv::Scalar(255, 0, 0, 0));
  }
  displayInNewWindow(displayName, combined);
}

void toHomogeneous2D(const std::vector<Eigen::Vector2d>& in, std::vector<Eigen::Vector3d>* result) {
  result->resize(in.size());
  std::transform(
      std::begin(in), std::end(in), std::begin(*result),
      [](const Eigen::Vector2d& value) { return Eigen::Vector3d(value.x(), value.y(), 1.0); });
}

// usually have to divide by z() but not necessary here
void toEuclidian2DTruncate(const std::vector<Eigen::Vector3d>& in,
                           std::vector<Eigen::Vector2d>* result) {
  result->resize(in.size());
  std::transform(
      std::begin(in), std::end(in), std::begin(*result),
      [](const Eigen::Vector3d& value) { return Eigen::Vector2d(value.x(), value.y()); });
}

std::ostream& operator<<(std::ostream& out, const s3d::StanAlignment& model) {
  out << "Vertical offset (%) = " << model.ch_y << std::endl;
  out << "Roll angle (degrees) = " << tan(model.a_z * 3.141592653589793 / 180.0) << std::endl;
  out << "Focal distance ratio (%) = " << (model.a_f + 1.0) * 100.0 << std::endl;
  out << "Tilt offset (pixels) = " << model.f_a_x << std::endl;
  out << "Pan keystone = " << model.a_x_f << std::endl;
  out << "Tilt offset keystone = " << model.a_y_f << std::endl;
  out << "Z parallax deformation =  " << model.ch_z_f << std::endl << std::endl;
  return out;
}

cv::Mat QImage2Mat(QImage const& src) {
  //  src.convertToFormat(QImage::Format_RGB888);

  cv::Mat tmp(src.height(), src.width(), CV_8UC4, const_cast<uchar*>(src.bits()),
              src.bytesPerLine());
  cv::Mat result;  // deep copy just in case (my lack of knowledge with open cv)
  cvtColor(tmp, result, CV_BGRA2GRAY);
  return result;
}
}  // namespace

void DepthAnalyzer::analyze(const QImage& imageLeft, const QImage& imageRight) {
  cv::Mat leftOrig = QImage2Mat(imageLeft);
  cv::Mat rightOrig = QImage2Mat(imageRight);

  //  ::displayInNewWindow("yo", leftOrig);

  constexpr float resizeRatio = 2.0f;

  cv::resize(leftOrig, leftOrig, cv::Size2f(static_cast<float>(leftOrig.cols) / resizeRatio,
                                            static_cast<float>(leftOrig.rows) / resizeRatio),
             cv::INTER_AREA);
  cv::resize(rightOrig, rightOrig, cv::Size2f(static_cast<float>(rightOrig.cols) / resizeRatio,
                                              static_cast<float>(rightOrig.rows) / resizeRatio),
             cv::INTER_AREA);

  // find matches
  std::unique_ptr<s3d::MatchFinder> matchFinder = std::make_unique<s3d::MatchFinderCV>();
  auto matches = matchFinder->findMatches({s3d::cv2image(leftOrig), s3d::cv2image(rightOrig)});
  assert(matches.size() == 2);
  std::vector<Eigen::Vector2d> pts1 = matches[0], pts2 = matches[1];

  // to homogeneous
  std::vector<Eigen::Vector3d> pts1h, pts2h;
  toHomogeneous2D(pts1, &pts1h);
  toHomogeneous2D(pts2, &pts2h);

  // display features
  cv::Mat leftClone = leftOrig.clone();
  cv::Mat rightClone = rightOrig.clone();

  //  drawFeatures(leftClone, pts1h);
  //  drawFeatures(rightClone, pts2h);
  //  displayInNewWindow("leftClone", leftClone);
  //  displayInNewWindow("rightClone", rightClone);

  // center points for ransac
  auto imageCenter = Eigen::Vector3d(leftOrig.rows / 2.0, leftOrig.cols / 2.0, 0.0);
  s3d::center_values(std::begin(pts1h), std::end(pts1h), std::begin(pts1h), imageCenter);
  s3d::center_values(std::begin(pts2h), std::end(pts2h), std::begin(pts2h), imageCenter);

  // solve F with RANSAC
  s3d::Ransac::Params params;
  params.nbTrials = 500;
  params.distanceThreshold =
      0.01 * sqrt(leftOrig.rows * leftOrig.rows + leftOrig.cols * leftOrig.cols);

  using s3d::RansacAlgorithm;
  using s3d::StanFundamentalMatrixSolver;
  using s3d::SampsonDistanceFunction;
  RansacAlgorithm<StanFundamentalMatrixSolver, SampsonDistanceFunction> ransac(params);

  auto model = ransac(pts1h, pts2h);
  //  std::cout << model;

  // display inliers (matches)
  std::vector<Eigen::Vector3d> bestPts1h, bestPts2h;
  std::tie(bestPts1h, bestPts2h) = ransac.getBestInlierSamples();

  s3d::center_values(std::begin(bestPts1h), std::end(bestPts1h), std::begin(bestPts1h),
                     -imageCenter);
  s3d::center_values(std::begin(bestPts2h), std::end(bestPts2h), std::begin(bestPts2h),
                     -imageCenter);

  // compute disparity range
  const float widthRatio = 100.0f / static_cast<float>(leftOrig.cols);

  std::vector<Eigen::Vector2d> bestPts1, bestPts2;
  toEuclidian2DTruncate(bestPts1h, &bestPts1);
  toEuclidian2DTruncate(bestPts2h, &bestPts2);
  std::vector<double> disparities;
  s3d::compute_disparities(bestPts1, bestPts2, back_inserter(disparities));

  // disparity histogram
  auto hist = Histogram::Compute(disparities, 10);

  double minDisp, maxDisp;
  std::tie(minDisp, maxDisp) = s3d::disparity_range(disparities);

  auto minDispP = minDisp * widthRatio;
  auto maxDispP = maxDisp * widthRatio;
  //  auto budgetP = (maxDisp - minDisp) * widthRatio;

  //  std::cout << "Disparity 1st percentile (%): " << minDispP << std::endl;
  //  std::cout << "Disparity 99th percentile (%): " << maxDispP << std::endl;
  //  std::cout << "Estimated disparity budget (%): " << budgetP << std::endl;
  //
  //  // disparity should generally not be more than 1/30 of screen width
  //  std::cout << "Suggested maximum disparity (%) : "
  //            << static_cast<float>(leftOrig.cols) / 30.0f * widthRatio << std::endl;

  // estimate pixel shift if current budget is considered adequate
  //  double wantedMinDisp = -1.0f;
  //  double wantedMaxDisp = 2.0f;
  //  double wantedCenter = (wantedMaxDisp + wantedMinDisp) / 2.0f;
  //  double currentCenter = (maxDispP + minDispP) / 2.0f;
  //  double shift = wantedCenter - currentCenter;

  //  std::cout << "Recommended shift (%) for " << wantedMinDisp << "% front, " << wantedMaxDisp
  //            << "% rear: " << shift << std::endl;
  //  std::cout << "Budget would become: " << minDispP + shift << ", " << maxDispP + shift <<
  //  std::endl;
  //  std::cout << "Recommended interaxial distance adjustment (%): "
  //            << budgetP - (wantedMaxDisp - wantedMinDisp) << std::endl;

  // Set outputs
  minDisparity = minDispP;
  maxDisparity = maxDispP;

  vertical = model.ch_y;
  roll = model.a_z;
  zoom = model.a_f;
  tiltOffset = model.f_a_x;
  panKeystone = model.a_x_f;
  tiltKeystone = model.a_y_f;

  featurePoints.clear();
  disparitiesPercent.clear();
  int i = 0;
  for (auto& pt : bestPts2) {
    featurePoints.emplace_back(pt.x() * resizeRatio, pt.y() * resizeRatio);
    disparitiesPercent.push_back(disparities[i++] * widthRatio);
  }
}
