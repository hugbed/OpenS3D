#ifndef WORKER_IMAGE_OPERATION_CHAIN_H
#define WORKER_IMAGE_OPERATION_CHAIN_H

#include "image_operation.h"

#include <vector>

namespace cv {
  class Mat;
}

namespace s3d {
namespace image_operation {

class ImageOperations {
public:
  bool applyAll(cv::Mat *leftImage, cv::Mat *rightImage);

  virtual bool shouldReturnAccordingTo(bool result) = 0;

  void setNext(ImageOperation *operation);
  const std::vector<ImageOperation*>& getAll();

private:
  std::vector<ImageOperation*> operations{};
};

class Sequence : public ImageOperations {
public:
  bool shouldReturnAccordingTo(bool result) override;
  bool shouldNeverReturn();
};

class Chain : public ImageOperations {
public:
  bool shouldReturnAccordingTo(bool operationSuccess) override;
  bool returnOnlyIf(bool operationSuccesful);
};

} // namespace s3d
} // namespace image_operation

#endif // WORKER_IMAGE_OPERATION_CHAIN_H
