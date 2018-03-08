#ifndef S3D_CV_FEATURES_MATCH_FINDER_CV_H
#define S3D_CV_FEATURES_MATCH_FINDER_CV_H

#include <s3d/features/match_finder.h>

#include <opencv2/core/mat.hpp>    // cv::Mat
#include <opencv2/core/types.hpp>  // cv::DMatch, cv::KeyPoint

#include "s3d/utilities/eigen.h"

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

  void setMaxNumberOfFeatures(int maxNumberOfFeatures);

  MatchFinder::Matches findMatches(const std::vector<Image<uint8_t>>& images) override;
  Features findFeatures(const Image<uint8_t>& img);

  MatchFinder::Matches findMatches(const cv::Mat& imageLeft, const cv::Mat& imageRight);
  Features findFeatures(const cv::Mat& img);

  MatchFinder::Matches matchFeatures(const Features& leftFeatures, const Features& rightFeatures);

  std::vector<cv::KeyPoint> keepBestKeypointsFromResponse(const std::vector<cv::KeyPoint>& keypoints, size_t numberToKeep);

  double matchesMinDistance(Matches matches) const;

  static double computeThreshold(int imageWidth, int imageHeight);

  virtual cv::Ptr<cv::Feature2D> createFeatureDetector();

  virtual cv::Ptr<cv::DescriptorMatcher> createDescriptorMatcher();

 protected:
  virtual void onFeaturesFound(const cv::Mat& /*imgLeft*/,
                               const cv::Mat& /*imgRight*/,
                               const MatchFinderCV::Features& /*featuresLeft*/,
                               const MatchFinderCV::Features& /*featuresRight*/) {}

  virtual void onFeaturesMatched(const cv::Mat& /*imgLeft*/,
                                 const cv::Mat& /*imgRight*/,
                                 const MatchFinder::Matches& /*matches*/) {}

  size_t maxNbFeatures_{3000}; // todo: set this somewhere else
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

  void onFeaturesMatched(const cv::Mat& imgLeft,
                         const cv::Mat& imgRight,
                         const MatchFinder::Matches& matches) override;

private:
  cv::Mat copyWithoutAlpha(const cv::Mat& img);
};

}  // namespace s3d

#endif  // S3D_CV_FEATURES_MATCH_FINDER_CV_H
