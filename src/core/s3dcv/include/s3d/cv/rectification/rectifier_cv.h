#ifndef S3D_CV_RECTIFIER_STAN_H
#define S3D_CV_RECTIFIER_STAN_H

#include "s3d/rectification/rectification_stan.h"

#include "s3d/cv/utilities/cv.h"

#include <Eigen/Dense>

namespace s3d {

class RectifierCV : public Rectifier {
 public:
  virtual gsl::owner<RectifierCV*> clone() const override { return new RectifierCV(); }

  template <class T>
  cv::Mat rectifyCV(const cv::Mat& image, const cv::Matx<T, 3, 3>& H) {
    cv::Mat imageWarped = image.clone();
    cv::warpPerspective(image, imageWarped, H, cv::Size(image.cols, image.rows));
    return imageWarped;
  }

  Image<uint8_t> rectify(const Image<uint8_t>& image, Eigen::Matrix3d H) override {
    // convert to cv::Mat
    auto H_CV = s3d::eigenMatToCV(H);
    cv::Mat imageCV = image2cv(image);

    // rectify using warp
    cv::Mat imageWarped = rectifyCV(imageCV, H_CV);

    // cv to eigen
    return cv2image(imageWarped);
  }
};

}  // namespace s3d

#endif  // S3D_CV_RECTIFIER_STAN_H
