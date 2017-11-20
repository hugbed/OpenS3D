#ifndef WORKER_IMAGE_OPERATION_INPUT_OUTPUT_ADAPTER_H
#define WORKER_IMAGE_OPERATION_INPUT_OUTPUT_ADAPTER_H

#include "image_operations.h"

#include <cv.h>

class ImageOperationsInputOutputAdapter {
public:
  explicit ImageOperationsInputOutputAdapter(gsl::not_null<ImageOperations*> imageOperations)
          : operations_{imageOperations} {}

  void setInputImages(const cv::Mat& leftImage, const cv::Mat& rightImage) {
    inputLeftImage = leftImage.clone();
    inputRightImage = rightImage.clone();
  }

  std::tuple<cv::Mat&, cv::Mat&> getOutputImages() {
    return { outputLeftImage, outputRightImage };
  };

  bool applyAllOperations() {
    copyInputToOutputs();
    if (!canApplyOperations()) {
      return false;
    }
    return operations_->applyAll(&outputLeftImage, &outputRightImage);
  }

  void copyInputToOutputs() {
    outputLeftImage = inputLeftImage.clone();
    outputRightImage = inputRightImage.clone();
  }

  bool canApplyOperations() {
    return !inputLeftImage.empty() && !inputRightImage.empty();
  }

private:
  cv::Mat inputLeftImage{};
  cv::Mat inputRightImage{};
  cv::Mat outputLeftImage{};
  cv::Mat outputRightImage{};

  ImageOperations* operations_{};
};

#endif //WORKER_IMAGE_OPERATION_INPUT_OUTPUT_ADAPTER_H
