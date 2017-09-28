#ifndef S3D_CV_UTILITIES_CV_H
#define S3D_CV_UTILITIES_CV_H

#include "s3d/image/image.h"
#include "s3d/video/video_frame.h"
#include "s3d/video/video_types.h"
#include "s3d/utilities/random_color_generator.h"

#include "Eigen/Dense"

#include <opencv2/opencv.hpp>
#include <opencv/cxeigen.hpp>

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
  return cv::Mat(size.getHeight(),
                 size.getWidth(),
                 nbChannelsToMatType(nbChannels),
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

inline void displayInNewWindow(const std::string& name, cv::InputArray src) {
  cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
  cv::imshow(name, src);
  cv::waitKey(0);
}

inline void drawMatches(const cv::Mat& left,
                        const cv::Mat& right,
                        const std::vector<Eigen::Vector2d>& featuresLeft,
                        const std::vector<Eigen::Vector2d>& featuresRight) {
  cv::Mat leftRight(left.rows, left.cols * 2, left.type());
  cv::hconcat(left, right, leftRight);

  // convert to color
  cv::cvtColor(leftRight, leftRight, cv::COLOR_GRAY2BGR);

  // for random color
  RandomColorGenerator<cv::Scalar> colorGenerator;

  for (int i = 0; i < featuresLeft.size(); ++i) {
    // random color
    cv::Scalar color = colorGenerator.randomColor();

    // discretize features
    const Eigen::Vector2i featureLeft = featuresLeft[i].cast<int>();
    const Eigen::Vector2i featureRight =
        (featuresRight[i] + Eigen::Vector2d(left.cols, 0.0f)).cast<int>();

    const cv::Point ptLeft(featureLeft.x(), featureLeft.y());
    const cv::Point ptRight(featureRight.x(), featureRight.y());

    cv::line(leftRight, ptLeft, ptRight, color, 1, CV_AA);
    cv::circle(leftRight, ptLeft, 3, color, 1, CV_AA);
    cv::circle(leftRight, ptRight, 3, color, 1, CV_AA);
  }

  displayInNewWindow("matches", leftRight);
}
}  // namespace s3d

#endif  // S3D_CV_UTILITIES_CV_H
