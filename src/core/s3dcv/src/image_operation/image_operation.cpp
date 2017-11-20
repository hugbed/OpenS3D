#include "s3d/cv/image_operation/image_operation.h"

#include <opencv2/core/mat.hpp>

namespace s3d {
namespace image_operation {

bool ImageOperation::applyOnImagesIfEnabled(cv::Mat* leftImage, cv::Mat* rightImage) {
  if (isEnabled) {
    return applyOnImage(leftImage, rightImage);
  }
  return false;
}

void ImageOperation::enable() {
  isEnabled = true;
}

void ImageOperation::disable() {
  isEnabled = false;
}

} // namespace s3d
} // namespace image_operation