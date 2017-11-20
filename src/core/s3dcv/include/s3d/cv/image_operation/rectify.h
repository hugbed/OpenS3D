#ifndef WORKER_IMAGEOPERATIONRECTIFY_H
#define WORKER_IMAGEOPERATIONRECTIFY_H

#include "image_operation.h"

#include <Eigen/Dense>

namespace s3d {
  class DisparityAnalyzerSTAN;
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
  bool applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage) override;

  s3d::DisparityAnalyzerSTAN* disparityAnalyzer_;
};

} // namespace s3d
} // namespace image_operation

#endif // WORKER_IMAGEOPERATIONRECTIFY_H
