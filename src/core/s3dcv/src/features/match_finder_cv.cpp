#include "s3d/cv/features/match_finder_cv.h"

#include "s3d/cv/utilities/cv.h"

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

  onFeaturesMatched(imageLeft, imageRight, matches);

  return matches;
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

MatchFinder::Matches MatchFinderCV::matchFeatures(const FeaturesCV& leftFeatures,
                                                  const FeaturesCV& rightFeatures) {
  std::vector<std::vector<cv::DMatch>> matches;
  auto matcher = createDescriptorMatcher();
  matcher->knnMatch(leftFeatures.descriptors, rightFeatures.descriptors, matches, 2);

  std::vector<Eigen::Vector2d> pts1, pts2;
  for (unsigned i = 0; i < matches.size(); i++) {
    if (matches[i][0].distance < 0.8f * matches[i][1].distance) {
      auto& pt1 = leftFeatures.keypoints[matches[i][0].queryIdx].pt;
      auto& pt2 = rightFeatures.keypoints[matches[i][0].trainIdx].pt;
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
  //  return cv::ORB::create(500, 1.2f, 32, 5, 0, 2, cv::ORB::HARRIS_SCORE, 31, 20);
  return cv::ORB::create();
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

void MatchFinderCVViz::onFeaturesMatched(const cv::Mat& imgLeft,
                                         const cv::Mat& imgRight,
                                         const MatchFinder::Matches& matches) {
  // recompute DMatches
  cv::Mat matchesImg;
  s3d::displayMatches("Filtered Matches", imgLeft, imgRight, matches[0], matches[1]);
}
}  // namespace s3d
