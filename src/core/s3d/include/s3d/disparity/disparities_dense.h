//
// Created by jon on 04/04/17.
//

#ifndef S3D_DISPARITY_DISPARITIES_DENSE_H
#define S3D_DISPARITY_DISPARITIES_DENSE_H

#include "disparities.h"

#include <cstdint>
#include <vector>

namespace s3d {

class DisparitiesDense : public Disparities {
 public:
  DisparitiesDense(Image disparityMap,
                   DisparityPoint minDisparity,
                   DisparityPoint maxDisparity)
      : disparityMap_(disparityMap), min_(minDisparity), max_(maxDisparity) {}

  virtual const std::vector<DisparityPoint>& getDisparities() override {
    if (disparities_.size() > 0)
      return disparities_;
    for (int i = 0; i < disparityMap_.mat.rows; ++i) {
      for (int j = 0; j < disparityMap_.mat.cols; ++j) {
        auto dx = disparityMap_.mat.at<uint8_t>(i, j);
        disparities_.emplace_back(DisparityPoint({i, j}, {0, dx}));
      }
    }
    return disparities_;
  }

  virtual Image getDisparityMap() override { return disparityMap_; }

  virtual DisparityPoint max() override { return max_; }

  virtual DisparityPoint min() override { return min_; }

 private:
  Image disparityMap_;
  std::vector<DisparityPoint> disparities_;
  DisparityPoint min_;
  DisparityPoint max_;
};
}

#endif  // S3D_DISPARITY_DISPARITIES_DENSE_H
