#ifndef S3D_CV_FEATURES_MATCH_FINDER_SURF_H
#define S3D_CV_FEATURES_MATCH_FINDER_SURF_H

#include "match_finder_cv.h"

#include "s3d/cv/utilities/cv.h"

#include <opencv2/xfeatures2d.hpp>

namespace s3d {

class MatchFinderSurf : public MatchFinderCV {
public:
  cv::Ptr<cv::Feature2D> createFeatureDetector() override {
    int minHessian = 400;
    return cv::xfeatures2d::SURF::create(minHessian, 4, 3, false, true);
  }

  cv::Ptr<cv::DescriptorMatcher> createDescriptorMatcher() override {
    return cv::FlannBasedMatcher::create();
  }
};

}

#endif  // S3D_CV_FEATURES_MATCH_FINDER_SURF_H
