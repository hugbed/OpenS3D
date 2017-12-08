#ifndef S3D_CV_IMAGE_OPERATION_UPDATE_RECTIFICATION_H
#define S3D_CV_IMAGE_OPERATION_UPDATE_RECTIFICATION_H

#include "image_operation.h"

#include <Eigen/Dense>

namespace s3d {
class DisparityAnalyzerSTAN;
struct StanResults;
}

namespace cv {
class Mat;
}

namespace s3d {
namespace image_operation {

class UpdateRectification : public ImageOperation {
public:
  explicit UpdateRectification(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer);

private:
  bool applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage, StanResults* results) override;

  s3d::DisparityAnalyzerSTAN* disparityAnalyzer_;
};

} // namespace s3d
} // namespace image_operation

#endif // S3D_CV_IMAGE_OPERATION_UPDATE_RECTIFICATION_H
