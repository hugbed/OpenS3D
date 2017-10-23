#include "s3d/cv/features/match_finder_cv.h"
#include "s3d/cv/utilities/cv.h"
#include "s3d/disparity/utilities.h"
#include "s3d/math.h"
#include "s3d/multiview/sampson_distance_function.h"
#include "s3d/multiview/stan_fundamental_matrix_solver.h"
#include "s3d/robust_estimation/ransac.h"
#include "s3d/utilities/histogram.h"

#include "gsl/gsl"

#include <chrono>

class BadNumberOfArgumentsException {};
class FileNotFoundException {};

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
    cv::line(combined,
             cv::Point(pt1.x(), pt1.y()),
             cv::Point(pt2.x(), pt2.y()),
             cv::Scalar(255, 0, 0, 0));
  }
  displayInNewWindow(displayName, combined);
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

int main(int argc, char* argv[]) {
  auto t1 = std::chrono::high_resolution_clock::now();

  auto input_args = gsl::span<char*>(argv, argc);
  if (input_args.size() != 3) {
    throw BadNumberOfArgumentsException{};
  }

  // load images
  auto path_left = std::string(input_args[1]);
  auto path_right = std::string(input_args[2]);

  auto leftOrig = cv::imread(path_left, CV_LOAD_IMAGE_GRAYSCALE);
  auto rightOrig = cv::imread(path_right, CV_LOAD_IMAGE_GRAYSCALE);
  if (leftOrig.data == nullptr || rightOrig.data == nullptr) {
    throw FileNotFoundException{};
  }

  cv::resize(leftOrig, leftOrig, cv::Size(leftOrig.cols / 2, leftOrig.rows / 2));
  cv::resize(rightOrig, rightOrig, cv::Size(rightOrig.cols / 2, rightOrig.rows / 2));

  // find matches
  std::unique_ptr<s3d::MatchFinder> matchFinder = std::make_unique<s3d::MatchFinderCVViz>();
  auto matches = matchFinder->findMatches({s3d::cv2image(leftOrig), s3d::cv2image(rightOrig)});
  assert(matches.size() == 2);
  std::vector<Eigen::Vector2d> pts1 = matches[0], pts2 = matches[1];

  // to homogeneous
  std::vector<Eigen::Vector3d> pts1h, pts2h;
  s3d::toHomogeneous2D(pts1, &pts1h);
  s3d::toHomogeneous2D(pts2, &pts2h);

  // center points for ransac
  auto imageCenter = Eigen::Vector3d(leftOrig.rows / 2.0, leftOrig.cols / 2.0, 0.0);
  s3d::center_values(std::begin(pts1h), std::end(pts1h), std::begin(pts1h), imageCenter);
  s3d::center_values(std::begin(pts2h), std::end(pts2h), std::begin(pts2h), imageCenter);

  // solve F with RANSAC
  s3d::Ransac::Params params;
  params.nbTrials = 2000;
  params.distanceThreshold =
      0.01 * sqrt(leftOrig.rows * leftOrig.rows + leftOrig.cols * leftOrig.cols);

  using s3d::RansacAlgorithm;
  using s3d::SampsonDistanceFunction;
  using s3d::StanFundamentalMatrixSolver;
  RansacAlgorithm<StanFundamentalMatrixSolver, SampsonDistanceFunction> ransac(params);

  std::cout << ransac(pts1h, pts2h);

  auto t2 = std::chrono::high_resolution_clock::now();
  std::cout << "Computation Time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms"
            << std::endl
            << std::endl;

  // display inliers (matches)
  std::vector<Eigen::Vector3d> bestPts1, bestPts2;
  std::tie(bestPts1, bestPts2) = ransac.getBestInlierSamples();

  s3d::center_values(std::begin(bestPts1), std::end(bestPts1), std::begin(bestPts1), -imageCenter);
  s3d::center_values(std::begin(bestPts2), std::end(bestPts2), std::begin(bestPts2), -imageCenter);

  // 3D homogeneous -> 2D
  s3d::toEuclidian2DTruncate(bestPts1, &pts1);
  s3d::toEuclidian2DTruncate(bestPts2, &pts2);

  cv::Mat leftMatchesImg = leftOrig.clone();
  cv::Mat rightMatchesImg = rightOrig.clone();
  s3d::displayMatches("matches", leftMatchesImg, rightMatchesImg, pts1, pts2);

  // compute disparity range
  const float widthRatio = 100.0f / static_cast<float>(leftOrig.cols);

  std::vector<double> disparities;
  s3d::compute_disparities(pts1, pts2, back_inserter(disparities));

  // disparity histogram
  auto hist = s3d::Histogram<double>::Compute(disparities, 10);

  double minDisp, maxDisp;
  std::tie(minDisp, maxDisp) = s3d::disparity_range(disparities);

  auto minDispP = minDisp * widthRatio;
  auto maxDispP = maxDisp * widthRatio;
  auto budgetP = (maxDisp - minDisp) * widthRatio;

  std::cout << "Disparity 1st percentile (%): " << minDispP << std::endl;
  std::cout << "Disparity 99th percentile (%): " << maxDispP << std::endl;
  std::cout << "Estimated disparity budget (%): " << budgetP << std::endl;

  // disparity should generally not be more than 1/30 of screen width
  std::cout << "Suggested maximum disparity (%) : "
            << static_cast<float>(leftOrig.cols) / 30.0f * widthRatio << std::endl;

  // estimate pixel shift if current budget is considered adequate
  double wantedMinDisp = -1.0f;
  double wantedMaxDisp = 2.0f;
  double wantedCenter = (wantedMaxDisp + wantedMinDisp) / 2.0f;
  double currentCenter = (maxDispP + minDispP) / 2.0f;
  double shift = wantedCenter - currentCenter;

  std::cout << "Recommended shift (%) for " << wantedMinDisp << "% front, " << wantedMaxDisp
            << "% rear: " << shift << std::endl;
  std::cout << "Budget would become: " << minDispP + shift << ", " << maxDispP + shift << std::endl;
  std::cout << "Recommended interaxial distance adjustment (%): "
            << budgetP - (wantedMaxDisp - wantedMinDisp) << std::endl;

  return 0;
}
