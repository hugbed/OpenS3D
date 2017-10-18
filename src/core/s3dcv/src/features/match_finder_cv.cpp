#include "s3d/cv/features/match_finder_cv.h"

#include "s3d/cv/utilities/cv.h"

#include <opencv/cv.hpp>
#include <opencv/cxcore.hpp>
#include <opencv2/features2d.hpp>

namespace s3d {

using FeaturesCV = MatchFinderCV::Features;
using MatchesCV = MatchFinderCV::Matches;

MatchFinder::Matches MatchFinderCV::findMatches(const cv::Mat& imageLeft,
                                                const cv::Mat& imageRight) {
  auto featuresLeft = findFeatures(imageLeft);
  auto featuresRight = findFeatures(imageRight);

  if (featuresLeft.descriptors.rows == 0 || featuresRight.descriptors.rows == 0 ||
      featuresLeft.keypoints.size() == 0 || featuresRight.keypoints.size() == 0) {
    return {{}, {}};
  }

  onFeaturesFound(imageLeft, imageRight, featuresLeft, featuresRight);

  auto matches = matchFeatures(featuresLeft, featuresRight);
  if (matches.empty()) {
    return {{}, {}};
  }

  double threshold = computeThreshold(imageLeft.cols, imageLeft.rows);
  auto filteredMatches = filterMatches(featuresLeft, featuresRight, matches, threshold);

  onFeaturesMatched(
      imageLeft, imageRight, matches, filteredMatches, featuresLeft, featuresRight, threshold);

  return filteredMatches;
}

MatchFinder::Matches MatchFinderCV::findMatches(const std::vector<Image<uint8_t>>& images) {
  assert(images.size() == 2);
  return findMatches(s3d::image2cv(images[0]), s3d::image2cv(images[1]));
}

FeaturesCV MatchFinderCV::findFeatures(const cv::Mat& img) {
  cv::Mat descriptors;
  std::vector<cv::KeyPoint> keypoints;
  auto featureDetector = createFeatureDetector();
  featureDetector->detectAndCompute(img, cv::noArray(), keypoints, descriptors);
  return {descriptors, keypoints};
}

FeaturesCV MatchFinderCV::findFeatures(const Image<uint8_t>& img) {
  return findFeatures(s3d::image2cv(img));
}

MatchesCV MatchFinderCV::matchFeatures(const FeaturesCV& leftFeatures,
                                       const FeaturesCV& rightFeatures) {
  std::vector<cv::DMatch> matches;
  auto matcher = createDescriptorMatcher();
  matcher->match(leftFeatures.descriptors, rightFeatures.descriptors, matches);
  return matches;
}

MatchFinder::Matches MatchFinderCV::filterMatches(const FeaturesCV& leftFeatures,
                                                  const FeaturesCV& rightFeatures,
                                                  const MatchesCV& matches,
                                                  double distanceThreshold) const {
  std::vector<Eigen::Vector2d> pts1;
  std::vector<Eigen::Vector2d> pts2;
  for (int i = 0; i < leftFeatures.descriptors.rows; i++) {
    if (matches[i].distance <= std::max(distanceThreshold, 0.02)) {
      auto& pt1 = leftFeatures.keypoints[matches[i].queryIdx].pt;
      auto& pt2 = rightFeatures.keypoints[matches[i].trainIdx].pt;
      pts1.emplace_back(pt1.x, pt1.y);
      pts2.emplace_back(pt2.x, pt2.y);
    }
  }
  return {pts1, pts2};
}

double MatchFinderCV::matchesMinDistance(MatchesCV matches) const {
  auto matchIsSmaller = [](const cv::DMatch& match1, const cv::DMatch& match2) {
    return match1.distance < match2.distance;
  };
  return std::min_element(std::begin(matches), std::end(matches), matchIsSmaller)->distance;
};

cv::Ptr<cv::Feature2D> MatchFinderCV::createFeatureDetector() {
  return cv::ORB::create(500, 1.2f, 32, 5, 0, 2, cv::ORB::HARRIS_SCORE, 31, 20);
}

cv::Ptr<cv::DescriptorMatcher> MatchFinderCV::createDescriptorMatcher() {
  return cv::DescriptorMatcher::create("BruteForce-Hamming(2)");
}

// static
double MatchFinderCV::computeThreshold(int imageWidth, int imageHeight) {
  double W = imageWidth;
  double H = imageHeight;
  return 0.07 * sqrt(W * W + H * H);
}

void MatchFinderCVViz::onFeaturesFound(const cv::Mat& imgLeft,
                                       const cv::Mat& imgRight,
                                       const MatchFinderCV::Features& featuresLeft,
                                       const MatchFinderCV::Features& featuresRight) {
  cv::Mat imageLeftWithKeypoints = imgLeft.clone();
  cv::Mat imageRightWithKeypoints = imgRight.clone();
  cv::drawKeypoints(imageLeftWithKeypoints, featuresLeft.keypoints, imageLeftWithKeypoints);
  cv::drawKeypoints(imageRightWithKeypoints, featuresRight.keypoints, imageRightWithKeypoints);

  cv::Mat images(imgLeft.rows, imgRight.cols * 2, CV_8UC3);
  cv::hconcat(imageLeftWithKeypoints, imageRightWithKeypoints, images);
  cv::imshow("Features Found", images);
}

void MatchFinderCVViz::onFeaturesMatched(cv::Mat imgLeft,
                                         cv::Mat imgRight,
                                         std::vector<cv::DMatch> matches,
                                         MatchFinder::Matches filteredMatches,
                                         const MatchFinderCV::Features& featuresLeft,
                                         const MatchFinderCV::Features& featuresRight,
                                         double threshold) {
  // recompute DMatches
  std::vector<cv::DMatch> filteredMatchesCV;
  for (int i = 0; i < featuresLeft.descriptors.rows; i++) {
    if (matches[i].distance <= std::max(threshold, 0.02)) {
      filteredMatchesCV.push_back(matches[i]);
    }
  }
  cv::Mat matchesImg;
  cv::drawMatches(imgLeft,
                  featuresLeft.keypoints,
                  imgRight,
                  featuresRight.keypoints,
                  filteredMatchesCV,
                  matchesImg);
  cv::imshow("Filtered Matches", matchesImg);
}
}  // namespace s3d
