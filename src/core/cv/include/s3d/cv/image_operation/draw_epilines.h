#ifndef S3D_CV_IMAGE_OPERATION_DRAW_EPILINES_H
#define S3D_CV_IMAGE_OPERATION_DRAW_EPILINES_H

#include "image_operation.h"

#include <s3d/geometry/size.h>

#include <Eigen/Dense>

namespace s3d {
  class DisparityAnalyzerSTAN;
}

namespace cv {
  class Mat;
}

namespace s3d {
namespace image_operation {

class DrawEpilines : public ImageOperation {
public:
  explicit DrawEpilines(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer);

private:
  bool applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage, StanResults* results) override;

  Eigen::Matrix3d getFundamentalMatrix(const Size& imageSize);

  s3d::DisparityAnalyzerSTAN* disparityAnalyzer_;
};

} // namespace s3d
} // namespace image_operation

#endif // S3D_CV_IMAGE_OPERATION_DRAW_EPILINES_H
