#ifndef WORKER_IMAGEOPERATIONCOMPUTEALIGNMENT_H
#define WORKER_IMAGEOPERATIONCOMPUTEALIGNMENT_H

#include "image_operation.h"

#include <s3d/cv/disparity/disparity_analyzer_stan.h>

class ImageOperationComputeAlignment : public ImageOperation {
public:
  explicit ImageOperationComputeAlignment(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer)
          : disparityAnalyzer_{disparityAnalyzer} {}

private:
  bool applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage) override {
    bool enoughFeaturePointsFound = disparityAnalyzer_->analyze(*leftImage, *rightImage);
    return enoughFeaturePointsFound; // && disparityRangeWideEnough();
  }

  bool disparityRangeWideEnough() {
    return (disparityAnalyzer_->results.maxDisparityPercent - disparityAnalyzer_->results.minDisparityPercent) > minDisparityRange_;
  }

  float minDisparityRange_{0.005};
  s3d::DisparityAnalyzerSTAN* disparityAnalyzer_;
};

#endif // WORKER_IMAGEOPERATIONCOMPUTEALIGNMENT_H
