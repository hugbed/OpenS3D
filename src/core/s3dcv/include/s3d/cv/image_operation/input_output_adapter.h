#ifndef WORKER_IMAGE_OPERATION_INPUT_OUTPUT_ADAPTER_H
#define WORKER_IMAGE_OPERATION_INPUT_OUTPUT_ADAPTER_H

#include <opencv2/core/mat.hpp>

#include <gsl/gsl>

namespace s3d {
namespace image_operation {

class ImageOperations;

class InputOutputAdapter {
public:
  InputOutputAdapter(gsl::not_null<ImageOperations*> imageOperations);

  void setInputImages(const cv::Mat& leftImage, const cv::Mat& rightImage);
  std::tuple<cv::Mat&, cv::Mat&> getOutputImages();
  bool applyAllOperations();
  void copyInputToOutputs();
  bool canApplyOperations();

private:
  cv::Mat inputLeftImage{};
  cv::Mat inputRightImage{};
  cv::Mat outputLeftImage{};
  cv::Mat outputRightImage{};

  ImageOperations* operations_{};
};

} // namespace s3d
} // namespace image_operation

#endif //WORKER_IMAGE_OPERATION_INPUT_OUTPUT_ADAPTER_H
