#ifndef WORKER_IMAGE_OPERATION_CHAIN_H
#define WORKER_IMAGE_OPERATION_CHAIN_H

#include "image_operation.h"

class ImageOperations {
public:
  bool applyAll(cv::Mat *leftImage, cv::Mat *rightImage) {
    for (auto* operation : operations) {
      bool result = operation->applyOnImagesIfEnabled(leftImage, rightImage);
      if (shouldReturnAccordingTo(result)) {
        return result;
      }
    }
    return true;
  }

  void setNext(ImageOperation *operation) {
    operations.push_back(operation);
  }

  const std::vector<ImageOperation*>& getAll() {
    return operations;
  }

  virtual bool shouldReturnAccordingTo(bool result) = 0;

private:
  std::vector<ImageOperation*> operations;
};

class ImageOperationSequence : public ImageOperations {
public:
  bool shouldReturnAccordingTo(bool result) override {
    return shouldNeverReturn(); // all operations are executed
  }

  bool shouldNeverReturn() {
    return false;
  }
};

class ImageOperationChain : public ImageOperations {
public:
  bool shouldReturnAccordingTo(bool operationSuccess) override {
    return returnOnlyIf(operationSuccess);
  }

  bool returnOnlyIf(bool operationSuccesful) {
    return operationSuccesful == false;
  }

};

// todo: have ImageOperationChain that is broken when an operation returns false (early return)

#endif // WORKER_IMAGE_OPERATION_CHAIN_H
