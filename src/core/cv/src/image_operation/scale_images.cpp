#include "s3d/cv/image_operation/scale_images.h"

#include "s3d/cv/utilities/cv.h"

namespace s3d {
namespace image_operation {

void ScaleImages::setRatio(float resizeRatio) {
  imageScaler_.setRatio(resizeRatio);
}

float ScaleImages::getRatio() {
  return imageScaler_.getRatio();
}

bool ScaleImages::applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage, StanResults* results) {
  if (leftImage->empty() || rightImage->empty()) {
    return false;
  }
  imageScaler_.resizePair(leftImage, rightImage);
  return true;
}

} // namespace s3d
} // namespace image_operation