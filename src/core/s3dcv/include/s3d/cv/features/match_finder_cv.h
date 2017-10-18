#ifndef S3D_CV_FEATURES_MATCH_FINDER_CV_H
#define S3D_CV_FEATURES_MATCH_FINDER_CV_H

#include "s3d/features/match_finder.h"

#include <opencv2/core/mat.hpp>    // cv::Mat
#include <opencv2/core/types.hpp>  // cv::DMatch, cv::KeyPoint

#include "Eigen/Dense"

#include <vector>

namespace cv {
class Feature2D;
class DescriptorMatcher;
}  // namespace cv

namespace s3d {

class MatchFinderCV : public MatchFinder {
 public:
  using Matches = std::vector<cv::DMatch>;

  struct Features {
    cv::Mat descriptors;
    std::vector<cv::KeyPoint> keypoints;
  };

  MatchFinder::Matches findMatches(const std::vector<Image<uint8_t>>& images) override;
  Features findFeatures(const Image<uint8_t>& img);

  MatchFinder::Matches findMatches(const cv::Mat& imageLeft, const cv::Mat& imageRight);
  Features findFeatures(const cv::Mat& img);

  Matches matchFeatures(const Features& leftFeatures, const Features& rightFeatures);

  MatchFinder::Matches filterMatches(const Features& leftFeatures,
                                     const Features& rightFeatures,
                                     const Matches& matches,
                                     double distanceThreshold) const;

  double matchesMinDistance(Matches matches) const;

  static double computeThreshold(int imageWidth, int imageHeight);

  virtual cv::Ptr<cv::Feature2D> createFeatureDetector();

  virtual cv::Ptr<cv::DescriptorMatcher> createDescriptorMatcher();

 protected:
  virtual void onFeaturesFound(const cv::Mat& imgLeft,
                               const cv::Mat& imgRight,
                               const MatchFinderCV::Features& featuresLeft,
                               const MatchFinderCV::Features& featuresRight) {}

  virtual void onFeaturesMatched(cv::Mat imgLeft,
                                 cv::Mat imgRight,
                                 std::vector<cv::DMatch> matches,
                                 MatchFinder::Matches filteredMatches,
                                 const MatchFinderCV::Features& featuresLeft,
                                 const MatchFinderCV::Features& featuresRight,
                                 double threshold) {}
};

/**
 *  MatchFinder with integrated visualization tools.
 */
class MatchFinderCVViz : public MatchFinderCV {
 protected:
  void onFeaturesFound(const cv::Mat& imgLeft,
                       const cv::Mat& imgRight,
                       const MatchFinderCV::Features& featuresLeft,
                       const MatchFinderCV::Features& featuresRight) override;

  void onFeaturesMatched(cv::Mat imgLeft,
                         cv::Mat imgRight,
                         std::vector<cv::DMatch> matches,
                         MatchFinder::Matches filteredMatches,
                         const MatchFinderCV::Features& featuresLeft,
                         const MatchFinderCV::Features& featuresRight,
                         double threshold) override;
};

}  // namespace s3d

#endif  // S3D_CV_FEATURES_MATCH_FINDER_CV_H
