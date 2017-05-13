#ifndef S3D_DISPARITY_DISPARITIES_H
#define S3D_DISPARITY_DISPARITIES_H

#include "s3d/image/image.h"

#include <cstddef>
#include <cv.h>  // todo: oh oh

namespace s3d {

struct ImageSize {
  size_t rows;
  size_t cols;
};

struct PixelPos {
  int row;
  int col;
};

struct DisparityPoint {
  DisparityPoint(PixelPos leftLoc, PixelPos disparityValue)
      : leftPos(leftLoc), disparity(disparityValue) {}

  PixelPos leftPos{};
  PixelPos disparity{};

  bool operator<(const DisparityPoint& d2) { return disparity.col < d2.disparity.col; }

  bool operator>(const DisparityPoint& d2) { return disparity.col > d2.disparity.col; }
};

class Disparities {
 public:
  virtual const std::vector<DisparityPoint>& getDisparities() = 0;
  virtual Image<uint8_t> getDisparityMap() = 0;
  virtual DisparityPoint max() = 0;
  virtual DisparityPoint min() = 0;
};
}  // namespace s3d

#endif  // S3D_DISPARITY_DISPARITIES_H
