#ifndef S3D_CV_IMAGE_OPERATION_IMAGE_OPERATION_H
#define S3D_CV_IMAGE_OPERATION_IMAGE_OPERATION_H

#include <gsl/gsl>

namespace cv {
  class Mat;
}

namespace s3d {
namespace image_operation {

class ImageOperation {
public:
  bool applyOnImagesIfEnabled(cv::Mat *leftImage, cv::Mat *rightImage);

  void enable();

  void disable();

private:
  virtual bool applyOnImage(cv::Mat *leftImage, cv::Mat *rightImage) = 0;

  bool isEnabled{true};
};

} // namespace s3d
} // namespace image_operation

#endif // S3D_CV_IMAGE_OPERATION_IMAGE_OPERATION_H
