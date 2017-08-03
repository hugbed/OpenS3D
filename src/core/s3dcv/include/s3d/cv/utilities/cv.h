#ifndef S3D_CV_UTILITIES_CV_H
#define S3D_CV_UTILITIES_CV_H

#include "s3d/image/image.h"
#include "s3d/video/video_types.h"
#include "s3d/video/video_frame.h"

#include "Eigen/Dense"

#include <opencv/cxcore.hpp>
#include <opencv/cxeigen.hpp>
#include <opencv2/imgproc.hpp>

#include <cstdint>

#include <gsl/gsl>

namespace s3d {

inline Image<uint8_t> cv2image(cv::Mat cvImg) {
  Image<uint8_t> img(Size(cvImg.cols, cvImg.rows));
  cv::cv2eigen(cvImg, img);
  return img;
}

inline cv::Mat image2cv(const Image<uint8_t>& img) {
  cv::Mat cvImg;
  cv::eigen2cv(img, cvImg);
  return cvImg;
}

inline int nbChannelsToMatType(int nbChannels) {
  int imgType = CV_8U;
  switch (nbChannels) {
    case 1:
      imgType = CV_8U;
      break;
    case 2:
      imgType = CV_8UC2;
      break;
    case 3:
      imgType = CV_8UC3;
      break;
    case 4:
      imgType = CV_8UC4;
      break;
    default:
      break;
  }
  return imgType;
}

inline cv::Mat dataToMat(Size size, VideoPixelFormat pixelFormat, gsl::span<const uint8_t> data) {
  auto nbChannels = static_cast<int>(VideoFrame::NumBytesPerPixel(pixelFormat));
  return cv::Mat(size.getHeight(), size.getWidth(), nbChannelsToMatType(nbChannels),
                 const_cast<uchar*>(data.data()));
}
// does not copy data, to copy use resizeMat(...).clone();
inline void resizeMat(gsl::not_null<cv::Mat*> mat, float colsRatio, float rowsRatio) {
  float cols = mat->cols;
  float rows = mat->rows;
  cv::resize(*mat, *mat, cv::Size2f(cols * colsRatio, rows * rowsRatio), cv::INTER_AREA);
}

inline void resizeMat(gsl::not_null<cv::Mat*> mat, float ratio) {
  resizeMat(mat, ratio, ratio);
}

}  // namespace s3d

#endif  // S3D_CV_UTILITIES_CV_H
