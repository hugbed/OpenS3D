#ifndef S3D_CV_IMAGE_OPERATION_RECTIFY_H
#define S3D_CV_IMAGE_OPERATION_RECTIFY_H

#include "image_operation.h"

#include "s3d/utilities/eigen.h"

namespace s3d {
  class DisparityAnalyzerSTAN;
  struct StanResults;
}

namespace cv {
  class Mat;
}

namespace s3d {
namespace image_operation {

class Rectify : public ImageOperation {
public:
  explicit Rectify(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer);

private:
  bool applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage, StanResults* results) override;

  void rectifyFeatures(StanResults* results);

  s3d::DisparityAnalyzerSTAN* disparityAnalyzer_;
};

} // namespace s3d
} // namespace image_operation

#endif // S3D_CV_IMAGE_OPERATION_RECTIFY_H
