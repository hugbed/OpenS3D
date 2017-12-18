#ifndef WORKER_SCALER_H
#define WORKER_SCALER_H

#include "s3d/cv/utilities/cv.h"

class Scaler {
public:
  void setRatio(float ratio) { ratio_ = ratio; }
  float getRatio() { return ratio_; }

  void resize(gsl::not_null<cv::Mat*> image) {
    if (0.0f < ratio_ && ratio_ < 0.99f) {
      s3d::resizeMat(image, ratio_);
    }
  }

  void resizePair(gsl::not_null<cv::Mat*> leftImage, gsl::not_null<cv::Mat*> rightImage) {
    resize(leftImage);
    resize(rightImage);
  }

private:
  float ratio_{1.0f};
};

#endif //WORKER_SCALER_H
