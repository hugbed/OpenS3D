#ifndef UTILITIES_CV_H
#define UTILITIES_CV_H

#include <opencv2/opencv.hpp>

inline cv::Mat QImage2Mat(QImage const& src) {
  cv::Mat tmp(
      src.height(), src.width(), CV_8UC4, const_cast<uchar*>(src.bits()), src.bytesPerLine());
  return tmp.clone();
}

inline QImage Mat2QImage(const cv::Mat& src) {
  QImage dest(const_cast<const uchar*>(src.data), src.cols, src.rows, src.step, QImage::Format_ARGB32);
  return dest.copy();
}

#endif  // UTILITIES_CV_H
