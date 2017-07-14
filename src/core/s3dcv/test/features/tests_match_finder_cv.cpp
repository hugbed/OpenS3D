#include "gtest/gtest.h"

#include "s3d/cv/features/match_finder_cv.h"

#include <opencv/cv.hpp>

#include <vector>

using s3d::MatchFinderCV;

class FakeFeatureDetector : public cv::Feature2D {
 public:
  FakeFeatureDetector() {
    goldKeypoints.emplace_back(cv::Point2f(1, 1), 1);
    goldKeypoints.emplace_back(cv::Point2f(1, 2), 2);
    goldDescriptors.emplace_back(0);
    goldDescriptors.emplace_back(1);
  }

  void detectAndCompute(cv::InputArray image,
                        cv::InputArray mask,
                        std::vector<cv::KeyPoint>& keypoints,
                        cv::OutputArray descriptors,
                        bool useProvidedKeypoints) override {
    // create fake keypoints
    keypoints.clear();
    keypoints.push_back(goldKeypoints[0]);
    keypoints.push_back(goldKeypoints[1]);

    // create fake descriptors
    descriptors.create(1, 2, CV_8U);
    cv::Mat outMat = descriptors.getMat();
    outMat.at<uchar>(0) = goldDescriptors[0];
    outMat.at<uchar>(1) = goldDescriptors[1];
  }

  std::vector<cv::KeyPoint> goldKeypoints;
  std::vector<uint8_t> goldDescriptors;
};

class FakeDescriptorMatcher : public cv::DescriptorMatcher {
 protected:
  void knnMatchImpl(const cv::_InputArray& queryDescriptors,
                    std::vector<std::vector<cv::DMatch>>& matches,
                    int k,
                    const cv::_InputArray& masks,
                    bool compactResult) override {
    matches.clear();
    std::vector<cv::DMatch> matchesVect;
    matchesVect.emplace_back(0, 0, 1.0);
    matches.push_back(matchesVect);
  }

  void radiusMatchImpl(const cv::_InputArray& queryDescriptors,
                       std::vector<std::vector<cv::DMatch>>& matches,
                       float maxDistance,
                       const cv::_InputArray& masks,
                       bool compactResult) override {
    matches.clear();
    std::vector<cv::DMatch> matchesVect;
    matchesVect.emplace_back(0, 0, 1.0);
    matches.push_back(matchesVect);
  }

  bool isMaskSupported() const override { return false; }

  cv::Ptr<cv::DescriptorMatcher> clone(bool emptyTrainData) const {
    return cv::Ptr<cv::DescriptorMatcher>(new FakeDescriptorMatcher);
  }
};

class FakeMatchFinderCV : public MatchFinderCV {
  cv::Ptr<cv::Feature2D> createFeatureDetector() override {
    return cv::Ptr<cv::Feature2D>(new FakeFeatureDetector);
  }

  cv::Ptr<cv::DescriptorMatcher> createDescriptorMatcher() override {
    return cv::Ptr<cv::DescriptorMatcher>(new FakeDescriptorMatcher);
  }
};

void expectSameKeypoint(const cv::KeyPoint& first, const cv::KeyPoint& second) {
  EXPECT_EQ(first.pt, second.pt);
  EXPECT_EQ(first.size, second.size);
  EXPECT_EQ(first.angle, second.angle);
  EXPECT_EQ(first.response, second.response);
  EXPECT_EQ(first.octave, second.octave);
  EXPECT_EQ(first.class_id, second.class_id);
}

TEST(match_finder_cv, find_features_returns_fake_features) {
  FakeMatchFinderCV matchFinder;
  auto features = matchFinder.findFeatures({});
  EXPECT_EQ(features.keypoints.size(), 2);

  FakeFeatureDetector goldFeatureDetector;
  for (int i = 0; i < features.keypoints.size(); ++i) {
    expectSameKeypoint(features.keypoints[i], goldFeatureDetector.goldKeypoints[i]);
  }
}

TEST(match_finder_cv, find_features_returns_fake_descriptors) {
  FakeMatchFinderCV matchFinder;
  auto features = matchFinder.findFeatures({});
  EXPECT_EQ(features.descriptors.cols, 2);

  FakeFeatureDetector goldFeatureDetector;
  for (int i = 0; i < features.descriptors.cols; ++i) {
    EXPECT_EQ(features.descriptors.at<uchar>(i), goldFeatureDetector.goldDescriptors[i]);
  }
}

TEST(match_finder_cv, filter_matches_returns_not_higher_than_threshold) {
  FakeMatchFinderCV matchFinder;
  auto featuresLeft = matchFinder.findFeatures({});
  auto featuresRight = matchFinder.findFeatures({});
  auto matches = matchFinder.matchFeatures(featuresLeft, featuresRight);
  auto matchesPositions = matchFinder.filterMatches(featuresLeft, featuresRight, matches, 0.0);
  EXPECT_EQ(matchesPositions.size(), 2);
  EXPECT_EQ(matchesPositions[0].size(), 0);
  EXPECT_EQ(matchesPositions[1].size(), 0);
}

void expectSamePoint(Eigen::Vector2d a, cv::Point2f b) {
  EXPECT_EQ(a.x(), b.x);
  EXPECT_EQ(a.y(), b.y);
}

TEST(match_finder_cv, filter_matches_returns_smaller_than_threshold) {
  FakeMatchFinderCV matchFinder;

  auto featuresLeft = matchFinder.findFeatures({});
  auto featuresRight = matchFinder.findFeatures({});
  auto matches = matchFinder.matchFeatures(featuresLeft, featuresRight);
  // alter features a bit
  featuresLeft.keypoints[0].pt = cv::Point2f(0, 0);
  featuresRight.keypoints[0].pt = cv::Point2f(1, 1);
  auto matchesPositions = matchFinder.filterMatches(featuresLeft, featuresRight, matches, 1.5);
  EXPECT_EQ(matchesPositions.size(), 2);
  EXPECT_EQ(matchesPositions[0].size(), 1);
  EXPECT_EQ(matchesPositions[1].size(), 1);
  expectSamePoint(matchesPositions[0][0], featuresLeft.keypoints[0].pt);
  expectSamePoint(matchesPositions[1][0], featuresRight.keypoints[0].pt);
}

TEST(match_finder_cv, matches_min_distance_returns_min) {
  constexpr double goldMinDist = 0.0;

  std::vector<cv::DMatch> matches;
  matches.emplace_back(0, 0, 2.0);
  matches.emplace_back(0, 0, goldMinDist);
  matches.emplace_back(0, 0, 3.0);

  FakeMatchFinderCV matchFinder;
  auto minDist = matchFinder.matchesMinDistance(matches);
  EXPECT_EQ(minDist, goldMinDist);
}

TEST(match_finder_cv, find_matches_returns_correct) {
  FakeMatchFinderCV matchFinder;
  auto res = matchFinder.findMatches(
      {s3d::Image<uint8_t>(Size(1920, 1080)), s3d::Image<uint8_t>(Size(1920, 1080))});

  EXPECT_EQ(res.size(), 2);
  ASSERT_EQ(res[0].size(), 1);
  ASSERT_EQ(res[1].size(), 1);

  auto goldPt = FakeFeatureDetector().goldKeypoints[0].pt;
  EXPECT_EQ(res[0][0], Eigen::Vector2d(goldPt.x, goldPt.y));
  EXPECT_EQ(res[1][0], Eigen::Vector2d(goldPt.x, goldPt.y));
}

TEST(match_finder_cv, cv_instance_creation_return_valid_objects) {
  MatchFinderCV m;
  EXPECT_NE(m.createFeatureDetector(), nullptr);
  EXPECT_NE(m.createDescriptorMatcher(), nullptr);
}
