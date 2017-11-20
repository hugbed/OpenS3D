#ifndef WORKER_IMAGEOPERATION_H
#define WORKER_IMAGEOPERATION_H

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

#endif //WORKER_IMAGEOPERATION_H
