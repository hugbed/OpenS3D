#ifndef UTILITIES_CV_H
#define UTILITIES_CV_H

#include <opencv2/opencv.hpp>

inline cv::Mat QImage2Mat(QImage const& src) {
  cv::Mat tmp(src.height(), src.width(), CV_8UC4, const_cast<uchar*>(src.bits()),
              src.bytesPerLine());
  cv::Mat result;  // deep copy just in case, todo: necessary?
  cvtColor(tmp, result, CV_BGRA2GRAY);
  return result;
}

#endif  // UTILITIES_CV_H
