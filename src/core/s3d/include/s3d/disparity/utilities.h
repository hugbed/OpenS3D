#ifndef S3D_DISPARITY_UTILITIES_H
#define S3D_DISPARITY_UTILITIES_H

#include "s3d/utilities/stats.h"

#include <vector>

namespace s3d {

// todo: unit test this

template <class OutIt>
void compute_disparities(const std::vector<Eigen::Vector2d>& positionsLeft,
                         const std::vector<Eigen::Vector2d>& positionsRight,
                         OutIt res) {
  assert(positionsLeft.size() == positionsRight.size());

  auto leftIt = std::begin(positionsLeft), rightIt = std::begin(positionsRight);
  for (; leftIt != std::end(positionsLeft); ++leftIt, ++rightIt) {
    *res++ = (*rightIt).x() - (*leftIt).x();
  }
};

inline std::pair<double, double> disparity_range(const std::vector<double>& disparities) {
  auto min = s3d::percentile(disparities, 0.03);
  auto max = s3d::percentile(disparities, 0.97);
  return {min, max};
};

inline std::pair<double, double> disparity_range(
    const std::vector<Eigen::Vector2d> positionsLeft,
    const std::vector<Eigen::Vector2d> positionsRight) {
  assert(positionsLeft.size() == positionsRight.size());

  std::vector<double> disparities;
  compute_disparities(positionsLeft, positionsRight, back_inserter(disparities));
  return disparity_range(disparities);
};

}  // namespace s3d

#endif  // S3D_DISPARITY_UTILITIES_H
