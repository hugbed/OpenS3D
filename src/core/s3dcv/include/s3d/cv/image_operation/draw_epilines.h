#ifndef WORKER_IMAGEOPERATIONDRAWEPILINES_H_H
#define WORKER_IMAGEOPERATIONDRAWEPILINES_H_H

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

class DrawEpilines : public ImageOperation {
public:
  explicit DrawEpilines(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer);

private:
  bool applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage) override;

  Eigen::Matrix3d getFundamentalMatrix();

  s3d::DisparityAnalyzerSTAN* disparityAnalyzer_;
};

} // namespace s3d
} // namespace image_operation

#endif // WORKER_IMAGEOPERATIONDRAWEPILINES_H_H
