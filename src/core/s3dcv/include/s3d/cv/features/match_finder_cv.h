#ifndef S3D_CV_MATCH_FINDER_CV_H
#define S3D_CV_MATCH_FINDER_CV_H

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

  Matches matchFeatures(const Features& leftFeatures, const Features& rightFeatures);

  MatchFinder::Matches filterMatches(const Features& leftFeatures,
                                     const Features& rightFeatures,
                                     const Matches& matches,
                                     double distanceThreshold) const;

  double matchesMinDistance(Matches matches) const;

  virtual cv::Ptr<cv::Feature2D> createFeatureDetector();

  virtual cv::Ptr<cv::DescriptorMatcher> createDescriptorMatcher();
};

}  // namespace s3d

#endif  // S3D_CV_MATCH_FINDER_CV_H
