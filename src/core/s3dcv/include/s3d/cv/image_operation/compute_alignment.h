#ifndef S3D_CV_IMAGE_OPERATION_COMPUTE_ALIGNMENT_H
#define S3D_CV_IMAGE_OPERATION_COMPUTE_ALIGNMENT_H

#include "image_operation.h"

namespace s3d {
  class DisparityAnalyzerSTAN;
  struct StanResults;
}

namespace cv {
  class Mat;
}

namespace s3d {
namespace image_operation {

class ComputeAlignment : public ImageOperation {
public:
  explicit ComputeAlignment(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer);

private:
  bool applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage, StanResults* results) override;
  bool disparityRangeWideEnough();

  float minDisparityRange_{0.1};
  s3d::DisparityAnalyzerSTAN* disparityAnalyzer_;
};

} // namespace s3d
} // namespace image_operation

#endif // S3D_CV_IMAGE_OPERATION_COMPUTE_ALIGNMENT_H
