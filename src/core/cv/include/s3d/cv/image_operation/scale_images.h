#ifndef S3D_CV_IMAGE_OPERATION_SCALE_IMAGES_H
#define S3D_CV_IMAGE_OPERATION_SCALE_IMAGES_H

#include "image_operation.h"

#include <s3d/cv/utilities/scaler.h>

namespace s3d {
class DisparityAnalyzerSTAN;
}

namespace cv {
class Mat;
}

namespace s3d {
namespace image_operation {

class ScaleImages : public ImageOperation {
public:
  void setRatio(float resizeRatio);

private:
  bool applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage, StanResults* results) override;

  Scaler imageScaler_{};
};

} // namespace s3d
} // namespace image_operation

#endif // S3D_CV_IMAGE_OPERATION_SCALE_IMAGES_H
