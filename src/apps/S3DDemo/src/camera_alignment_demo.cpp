#include "s3d/robust_estimation/ransac.h"
#include "s3d/multiview/sampson_distance_function.h"
#include "s3d/multiview/stan_fundamental_matrix_solver.h"

#include <opencv2/opencv.hpp>
#include <gsl/gsl>

#include <chrono>

class BadNumberOfArgumentsException {};
class FileNotFoundException {};

void displayInNewWindow(const std::string& name, cv::InputArray src) {
  //  cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
  //  cv::imshow(name, src);
  //  cv::waitKey(0);
}

int main(int argc, char* argv[]) {
  auto t1 = std::chrono::high_resolution_clock::now();

  auto input_args = gsl::span<char*>(argv, argc);
  if (input_args.size() == 2) {
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

  printf("-- Max dist : %f \n", max_dist);
  printf("-- Min dist : %f \n", min_dist);

  cv::Mat imgMatches;
  cv::drawMatches(leftOrig, keypoints1, rightOrig, keypoints2, goodMatches, imgMatches);
  //  cv::imshow( "Good Matches", imgMatches);
  cv::waitKey(0);

  cv::Mat leftCircles = leftOrig.clone();
  cv::Mat rightCircles = rightOrig.clone();

  for (int i = 0; i < pts1.size(); ++i) {
    auto& pt1 = pts1[i];
    auto& pt2 = pts2[i];

    cv::circle(leftCircles, cv::Point(pt1.x(), pt1.y()), 2, cv::Scalar(255, 0, 0, 0), -1);
    cv::circle(rightCircles, cv::Point(pt2.x(), pt2.y()), 2, cv::Scalar(255, 0, 0, 0), -1);
  }

  cv::Mat combinedAfter(leftCircles.rows, leftCircles.cols * 2, leftCircles.type());
  cv::hconcat(leftCircles, rightCircles, combinedAfter);
  displayInNewWindow("after", combinedAfter);

  // center points for ransac
  for (int i = 0; i < pts1.size(); ++i) {
    pts1[i] -= Eigen::Vector3d(leftCircles.rows / 2.0, leftCircles.cols / 2.0, 0.0);
    pts2[i] -= Eigen::Vector3d(rightCircles.rows / 2.0, rightCircles.cols / 2.0, 0.0);
  }

  s3d::Ransac::Params params;
  params.minNbPts = 5;
  params.nbTrials = 2000;
  params.distanceThreshold =
      0.01 * sqrt(leftOrig.rows * leftOrig.rows + leftOrig.cols * leftOrig.cols);
  s3d::RansacAlgorithm<s3d::StanFundamentalMatrixSolver, s3d::SampsonDistanceFunction> ransac(
      params);

  auto model = ransac(pts1, pts2);

  std::cout << model.ch_y << std::endl;
  std::cout << tan(model.a_z * 3.141592653589793 / 180.0) << std::endl;
  std::cout << (model.a_f + 1.0) * 100.0 << std::endl;
  std::cout << model.f_a_x << std::endl;
  std::cout << model.a_x_f << std::endl;
  std::cout << model.a_y_f << std::endl;
  std::cout << model.ch_z_f << std::endl;

  auto bestPts = ransac.getBestInlierPoints();

  auto bestPts1 = bestPts.first;
  auto bestPts2 = bestPts.second;

  std::cout << leftOrig.rows / 2.0 << std::endl;
  std::cout << leftOrig.cols / 2.0 << std::endl;

  // decenter points for ransac
  for (int i = 0; i < bestPts1.size(); ++i) {
    bestPts1[i] += Eigen::Vector3d(leftOrig.rows / 2.0, leftOrig.cols / 2.0, 0.0);
    bestPts2[i] += Eigen::Vector3d(rightOrig.rows / 2.0, rightOrig.cols / 2.0, 0.0);
    assert(bestPts1[i].x() >= 0.0);
    assert(bestPts1[i].y() >= 0.0);
  }

  cv::Mat leftMatches = leftOrig.clone();
  cv::Mat rightMatches = rightOrig.clone();

  for (int i = 0; i < bestPts1.size(); ++i) {
    auto& pt1 = bestPts1[i];
    auto& pt2 = bestPts2[i];

    cv::circle(leftMatches, cv::Point(pt1.x(), pt1.y()), 2, cv::Scalar(255, 0, 0, 0), -1);
    cv::circle(rightMatches, cv::Point(pt2.x(), pt2.y()), 2, cv::Scalar(255, 0, 0, 0), -1);
  }

  cv::Mat combinedWayAfter(leftMatches.rows, leftMatches.cols * 2, leftMatches.type());
  cv::hconcat(leftMatches, rightMatches, combinedWayAfter);

  for (int i = 0; i < bestPts1.size(); ++i) {
    auto& pt1 = bestPts1[i];
    auto& pt2 = bestPts2[i] + Eigen::Vector3d(leftMatches.cols, 0.0, 0.0);

    cv::line(combinedWayAfter, cv::Point(pt1.x(), pt1.y()), cv::Point(pt2.x(), pt2.y()),
             cv::Scalar(255, 0, 0, 0));
  }
  displayInNewWindow("after", combinedWayAfter);

  auto t2 = std::chrono::high_resolution_clock::now();

  std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
            << std::endl;

  return 0;
}
