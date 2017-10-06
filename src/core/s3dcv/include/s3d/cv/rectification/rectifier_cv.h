#ifndef S3D_CV_RECTIFIER_STAN_H
#define S3D_CV_RECTIFIER_STAN_H

#include "s3d/rectification/rectification_stan.h"

#include "s3d/cv/utilities/cv.h"

#include <Eigen/Dense>

namespace s3d {

class RectifierCV : public Rectifier {
 public:
  Image<uint8_t> rectify(const Image<uint8_t>& image, Eigen::Matrix3d H) override {
    // convert to cv::Mat
    cv::Mat H_CV;
    cv::eigen2cv(H, H_CV);
    cv::Mat imageCV = image2cv(image);

    // rectify using warp
    cv::Mat imageWarped = rectifyCV(imageCV, H_CV);

    // cv to eigen
    return cv2image(imageWarped);
  }

  cv::Mat rectifyCV(const cv::Mat& image, const cv::Mat& H) {
    cv::Mat imageWarped = image.clone();
    cv::warpPerspective(image, imageWarped, H, cv::Size(image.cols, image.rows));
    return imageWarped;
  }
};

}  // namespace s3d

#endif  // S3D_CV_RECTIFIER_STAN_H
