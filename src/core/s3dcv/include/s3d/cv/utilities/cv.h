#ifndef S3DCV_UTILITIES_CV_H
#define S3DCV_UTILITIES_CV_H

#include "s3d/image/image.h"

#include "Eigen/Dense"

#include <opencv/cxcore.hpp>
#include <opencv/cxeigen.hpp>

namespace s3d {

inline Image<uint8_t> cv2image(cv::Mat cvImg) {
  Image<uint8_t> img(Size(cvImg.rows, cvImg.cols));
  cv::cv2eigen(cvImg, img);
  return img;
}

inline cv::Mat image2cv(const Image<uint8_t>& img) {
  cv::Mat cvImg;
  cv::eigen2cv(img, cvImg);
  return cvImg;
}

}  // namespace s3d

#endif  // S3DCV_UTILITIES_CV_H
