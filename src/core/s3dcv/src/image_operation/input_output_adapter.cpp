#include "s3d/cv/image_operation/input_output_adapter.h"

#include "s3d/cv/image_operation/image_operations.h"

namespace s3d {
namespace image_operation {

InputOutputAdapter::InputOutputAdapter(gsl::not_null<ImageOperations*> imageOperations)
  : operations_{imageOperations} {}

void InputOutputAdapter::setInputImages(const cv::Mat& leftImage, const cv::Mat& rightImage) {
  inputLeftImage = leftImage.clone();
  inputRightImage = rightImage.clone();
}

std::tuple<cv::Mat&, cv::Mat&> InputOutputAdapter::getOutputImages() {
  return std::tuple<cv::Mat&, cv::Mat&>(outputLeftImage, outputRightImage);
};

bool InputOutputAdapter::applyAllOperations() {
  copyInputToOutputs();
  if (!canApplyOperations()) {
    return false;
  }
  return operations_->applyAll(&outputLeftImage, &outputRightImage);
}

void InputOutputAdapter::copyInputToOutputs() {
  outputLeftImage = inputLeftImage.clone();
  outputRightImage = inputRightImage.clone();
}

bool InputOutputAdapter::canApplyOperations() {
  return !inputLeftImage.empty() && !inputRightImage.empty();
}

} // namespace s3d
} // namespace image_operation