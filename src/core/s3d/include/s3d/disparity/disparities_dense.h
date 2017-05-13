#ifndef S3D_DISPARITY_DISPARITIES_DENSE_H
#define S3D_DISPARITY_DISPARITIES_DENSE_H

#include "disparities.h"

#include <cstdint>
#include <vector>

namespace s3d {

class DisparitiesDense : public Disparities {
 public:
  DisparitiesDense(Image<uint8_t> disparityMap,
                   DisparityPoint minDisparity,
                   DisparityPoint maxDisparity)
      : disparityMap_(std::move(disparityMap)), min_(minDisparity), max_(maxDisparity) {}

  const std::vector<DisparityPoint>& getDisparities() override {
    if (!disparities_.empty()) {
      return disparities_;
    }
    for (int i = 0; i < disparityMap_.height(); ++i) {
      for (int j = 0; j < disparityMap_.width(); ++j) {
        auto dx = disparityMap_(i, j);
        disparities_.emplace_back(DisparityPoint({i, j}, {0, dx}));
      }
    }
    return disparities_;
  }

  Image<uint8_t> getDisparityMap() override { return disparityMap_; }

  DisparityPoint max() override { return max_; }

  DisparityPoint min() override { return min_; }

 private:
  Image<uint8_t> disparityMap_;
  std::vector<DisparityPoint> disparities_;
  DisparityPoint min_;
  DisparityPoint max_;
};
}  // namespace s3d

#endif  // S3D_DISPARITY_DISPARITIES_DENSE_H
