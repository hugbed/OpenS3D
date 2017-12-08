#include "s3d/cv/image_operation/compute_alignment.h"

#include "s3d/cv/disparity/disparity_analyzer_stan.h"

#include <opencv2/core/mat.hpp>

namespace s3d {
namespace image_operation {

ComputeAlignment::ComputeAlignment(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer)
  : disparityAnalyzer_{disparityAnalyzer} {}

bool ComputeAlignment::applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage, StanResults* results) {
  bool enoughFeaturePointsFound = disparityAnalyzer_->analyze(*leftImage, *rightImage);
  return enoughFeaturePointsFound && disparityRangeWideEnough();
}

bool ComputeAlignment::disparityRangeWideEnough() {
  return (disparityAnalyzer_->results.maxDisparityPercent - disparityAnalyzer_->results.minDisparityPercent) > minDisparityRange_;
}

} // namespace s3d
} // namespace image_operation