#ifndef WORKER_IMAGEOPERATION_H
#define WORKER_IMAGEOPERATION_H

#include "utilities/cv.h"

#include <gsl/gsl>

namespace cv {
  class Mat;
}

class ImageOperation {
public:
  bool applyOnImagesIfEnabled(cv::Mat* leftImage, cv::Mat* rightImage) {
    if (isEnabled) {
      return applyOnImage(leftImage, rightImage);
    }
    return false;
  }

  void enable() { isEnabled = true; }
  void disable() { isEnabled = false; }

private:
  virtual bool applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage) = 0;

  bool isEnabled{true};
};



#endif //WORKER_IMAGEOPERATION_H
