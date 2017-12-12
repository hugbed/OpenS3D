#ifndef S3D_CV_IMAGE_OPERATION_FILTER_ALIGNMENT_H
#define S3D_CV_IMAGE_OPERATION_FILTER_ALIGNMENT_H

#include "image_operation.h"

#include "s3d/filter/kalman.h"

namespace s3d {
class DisparityAnalyzerSTAN;
struct StanResults;
}

namespace cv {
class Mat;
}

namespace s3d {
namespace image_operation {

class FilterAlignment : public ImageOperation {
public:
  explicit FilterAlignment(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer);

  void resetFilter();

private:
  bool applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage, StanResults* results) override;

  s3d::DisparityAnalyzerSTAN* disparityAnalyzer_;
  s3d::filter::Kalman kalmanFilter_;
};

} // namespace s3d
} // namespace image_operation

#endif // S3D_CV_IMAGE_OPERATION_FILTER_ALIGNMENT_H
