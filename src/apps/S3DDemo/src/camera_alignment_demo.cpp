#include "s3d/robust_estimation/ransac.h"
#include "s3d/multiview/sampson_distance_function.h"
#include "s3d/multiview/stan_fundamental_matrix_solver.h"

#include <opencv2/opencv.hpp>
#include <gsl/gsl>

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
    auto& pt2 = ptsRight[i] + Eigen::Vector3d(imgLeft.cols, 0.0, 0.0);
    cv::line(combined, cv::Point(pt1.x(), pt1.y()), cv::Point(pt2.x(), pt2.y()),
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

  auto path_left = std::string(input_args[1]);
  auto path_right = std::string(input_args[2]);

  auto leftOrig = cv::imread(path_left, CV_LOAD_IMAGE_GRAYSCALE);
  auto rightOrig = cv::imread(path_right, CV_LOAD_IMAGE_GRAYSCALE);
  if (leftOrig.data == nullptr || rightOrig.data == nullptr) {
    throw FileNotFoundException{};
  }

  cv::resize(leftOrig, leftOrig, cv::Size(leftOrig.cols / 2, leftOrig.rows / 2));
  cv::resize(rightOrig, rightOrig, cv::Size(rightOrig.cols / 2, rightOrig.rows / 2));

  // debugging
  //  cv::Mat combined(left.rows, left.cols * 2, left.type());
  //  cv::hconcat(left, right, combined);
  //  displayInNewWindow("before", combined);

  // detect features
  std::vector<cv::KeyPoint> keypoints1, keypoints2;

  cv::Mat descriptors1, descriptors2;

  auto orb = cv::ORB::create();
  orb->detectAndCompute(leftOrig, cv::noArray(), keypoints1, descriptors1);
  orb->detectAndCompute(rightOrig, cv::noArray(), keypoints2, descriptors2);

  auto matcher = cv::DescriptorMatcher::create("BruteForce-Hamming");
  std::vector<cv::DMatch> matches;
  matcher->match(descriptors1, descriptors2, matches);

  //  cv::Mat imgMatchesNotGood;
  //  cv::drawMatches(leftOrig, keypoints1, rightOrig, keypoints2, matches, imgMatchesNotGood);
  //  cv::imshow( "Not Good Matches", imgMatchesNotGood);
  //  cv::waitKey(0);

  double max_dist = 0;
  double min_dist = 100;
  //-- Quick calculation of max and min distances between keypoints
  for (int i = 0; i < descriptors1.rows; i++) {
    double dist = matches[i].distance;
    if (dist < min_dist)
      min_dist = dist;
    if (dist > max_dist)
      max_dist = dist;
  }

  std::vector<Eigen::Vector3d> pts1;
  std::vector<Eigen::Vector3d> pts2;

  std::vector<cv::DMatch> goodMatches;
  for (int i = 0; i < descriptors1.rows; i++) {
    if (matches[i].distance <= std::max(10 * min_dist, 0.02)) {
      goodMatches.push_back(matches[i]);

      cv::Point2f pt1 = keypoints1[matches[i].queryIdx].pt;
      cv::Point2f pt2 = keypoints2[matches[i].trainIdx].pt;

      pts1.emplace_back(pt1.x, pt1.y, 1.0);
      pts2.emplace_back(pt2.x, pt2.y, 1.0);
    }
  }

  //  cv::Mat imgMatches;
  //  cv::drawMatches(leftOrig, keypoints1, rightOrig, keypoints2, goodMatches, imgMatches);
  //  cv::imshow( "Good Matches", imgMatches);
  //  cv::waitKey(0);

  // // display found features
  //  cv::Mat leftCircles = leftOrig.clone();
  //  cv::Mat rightCircles = rightOrig.clone();
  //  drawFeatures(leftCircles, pts1);
  //  drawFeatures(rightCircles, pts2);
  //
  //  cv::Mat combinedAfter(leftCircles.rows, leftCircles.cols * 2, leftCircles.type());
  //  cv::hconcat(leftCircles, rightCircles, combinedAfter);
  //  displayInNewWindow("after", combinedAfter);

  // center points for ransac
  auto imageCenter = Eigen::Vector3d(leftOrig.rows / 2.0, leftOrig.cols / 2.0, 0.0);
  s3d::center_values(std::begin(pts1), std::end(pts1), std::begin(pts1), imageCenter);
  s3d::center_values(std::begin(pts2), std::end(pts2), std::begin(pts2), imageCenter);

  s3d::Ransac::Params params;
  params.minNbPts = 5;
  params.nbTrials = 2000;
  params.distanceThreshold =
      0.01 * sqrt(leftOrig.rows * leftOrig.rows + leftOrig.cols * leftOrig.cols);

  using s3d::RansacAlgorithm;
  using s3d::StanFundamentalMatrixSolver;
  using s3d::SampsonDistanceFunction;
  RansacAlgorithm<StanFundamentalMatrixSolver, SampsonDistanceFunction> ransac(params);

  std::cout << ransac(pts1, pts2);

  std::vector<Eigen::Vector3d> bestPts1, bestPts2;
  std::tie(bestPts1, bestPts2) = ransac.getBestInlierPoints();

  // decenter points after ransac
  s3d::center_values(std::begin(bestPts1), std::end(bestPts1), std::begin(bestPts1), -imageCenter);
  s3d::center_values(std::begin(bestPts2), std::end(bestPts2), std::begin(bestPts2), -imageCenter);

  // display inliers (matches)
  cv::Mat leftMatchesImg = leftOrig.clone();
  cv::Mat rightMatchesImg = rightOrig.clone();
  displayMatches("after", leftMatchesImg, rightMatchesImg, bestPts1, bestPts2);

  auto t2 = std::chrono::high_resolution_clock::now();

  std::cout << "Computation Time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms"
            << std::endl;

  return 0;
}
