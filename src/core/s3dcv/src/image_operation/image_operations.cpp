#include "s3d/cv/image_operation/image_operations.h"

#include <s3d/multiview/stan_results.h>

#include <opencv2/core/mat.hpp>

namespace s3d {
namespace image_operation {

bool ImageOperations::applyAll(cv::Mat *leftImage, cv::Mat *rightImage, StanResults* results) {
  for (auto* operation : operations) {
    bool result = operation->applyOnImagesIfEnabled(leftImage, rightImage, results);
    if (shouldReturnAccordingTo(result)) {
      return result;
    }
  }
  return true;
}

void ImageOperations::setNext(ImageOperation *operation) {
  operations.push_back(operation);
}

const std::vector<ImageOperation*>& ImageOperations::getAll() {
  return operations;
}


bool Sequence::shouldReturnAccordingTo(bool result) {
  return shouldNeverReturn(); // all operations are executed
}

bool Sequence::shouldNeverReturn() {
  return false;
}


bool Chain::shouldReturnAccordingTo(bool operationSuccess) {
  return returnOnlyIf(operationSuccess);
}

bool Chain::returnOnlyIf(bool operationSuccesful) {
  return operationSuccesful == false;
}

} // namespace s3d
} // namespace image_operation