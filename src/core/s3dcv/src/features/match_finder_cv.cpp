#include "s3d/cv/features/match_finder_cv.h"

#include "s3d/cv/utilities/cv.h"

#include <opencv/cxcore.hpp>
#include <opencv2/features2d.hpp>
#include <opencv/cv.hpp>

namespace s3d {

using FeaturesCV = MatchFinderCV::Features;
using MatchesCV = MatchFinderCV::Matches;

MatchFinder::Matches MatchFinderCV::findMatches(const std::vector<Image<uint8_t>>& images) {
  assert(images.size() == 2);

  auto imgIt = std::begin(images);
  auto featuresLeft = findFeatures(*imgIt++);
  auto featuresRight = findFeatures(*imgIt++);

  if (featuresLeft.descriptors.rows == 0 || featuresRight.descriptors.rows == 0 ||
      featuresLeft.keypoints.size() == 0 || featuresRight.keypoints.size() == 0) {
    return {{}, {}};
  }

  auto matches = matchFeatures(featuresLeft, featuresRight);

  if (matches.empty()) {
    return {{}, {}};
  }

  displayKeypoints(image2cv(images[0]), image2cv(images[1]), featuresLeft, featuresRight);

  auto filteredMatches = filterMatches(featuresLeft, featuresRight, matches,
                                       computeThreshold(images[0].width(), images[0].height()));

  // display matches
  std::vector<cv::DMatch> filteredMatchesCV;
  for (int i = 0; i < featuresLeft.descriptors.rows; i++) {
    if (matches[i].distance <=
        std::max(computeThreshold(images[0].width(), images[0].height()), 0.02)) {
      filteredMatchesCV.push_back(matches[i]);
    }
  }

  cv::Mat matchesImg;
  cv::drawMatches(image2cv(images[0]), featuresLeft.keypoints, image2cv(images[1]),
                  featuresRight.keypoints, filteredMatchesCV, matchesImg);
  cv::imshow("filtered matches", matchesImg);

  return filteredMatches;
}

FeaturesCV MatchFinderCV::findFeatures(const Image<uint8_t>& img) {
  cv::Mat descriptors;
  std::vector<cv::KeyPoint> keypoints;
  auto featureDetector = createFeatureDetector();
  featureDetector->detectAndCompute(s3d::image2cv(img), cv::noArray(), keypoints, descriptors);
  return {descriptors, keypoints};
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
  return 0.25 * sqrt(W * W + H * H);
}

void MatchFinderCV::displayKeypoints(const cv::Mat& imgLeft,
                                     const cv::Mat& imgRight,
                                     const MatchFinderCV::Features& featuresLeft,
                                     const MatchFinderCV::Features& featuresRight) {
  // debug keypoints
  cv::Mat imageLeftWithKeypoints = imgLeft.clone();
  cv::Mat imageRightWithKeypoints = imgRight.clone();
  cv::drawKeypoints(imageLeftWithKeypoints, featuresLeft.keypoints, imageLeftWithKeypoints);
  cv::drawKeypoints(imageRightWithKeypoints, featuresRight.keypoints, imageRightWithKeypoints);
  cv::imshow("left image", imageLeftWithKeypoints);
  cv::imshow("right image", imageRightWithKeypoints);
}

}  // namespace s3d
