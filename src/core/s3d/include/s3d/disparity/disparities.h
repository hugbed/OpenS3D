//
// Created by jon on 04/04/17.
//

#ifndef PROJECT_DISPARITIES_H
#define PROJECT_DISPARITIES_H

#include <cv.h>  // todo: oh oh
#include <stddef.h>

namespace s3d {

struct ImageSize {
  size_t rows;
  size_t cols;
};

struct Image {
  Image(cv::Mat imgMat) : mat(imgMat) {}
  cv::Mat mat;
};

struct PixelPos {
  int row;
  int col;
};

struct DisparityPoint {
  DisparityPoint(PixelPos leftLoc, PixelPos disparityValue)
      : leftPos(leftLoc), disparity(disparityValue) {}

  PixelPos leftPos;
  PixelPos disparity;

  bool operator<(const DisparityPoint& d2) {
    return disparity.col < d2.disparity.col;
  }

  bool operator>(const DisparityPoint& d2) {
    return disparity.col > d2.disparity.col;
  }
};

class Disparities {
 public:
  virtual const std::vector<DisparityPoint>& getDisparities() = 0;
  virtual Image getDisparityMap() = 0;
  virtual DisparityPoint max() = 0;
  virtual DisparityPoint min() = 0;
};
}

#endif  // PROJECT_DISPARITIES_H
