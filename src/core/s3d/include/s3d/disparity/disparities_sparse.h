#ifndef S3D_DISPARITY_DISPARITIES_SPARSE_H
#define S3D_DISPARITY_DISPARITIES_SPARSE_H

#include "disparities.h"
#include "s3d/geometry/size.h"

#include <vector>

namespace s3d {

class DisparitiesSparse : public Disparities {
 public:
  using DisparityPoints = std::vector<DisparityPoint>;

  DisparitiesSparse(DisparityPoints disparities, Size imageSize)
      : disparities_(std::move(disparities)), disparityMap_(imageSize) {}

  const std::vector<DisparityPoint>& getDisparities() override  // const & ??
  {
    return disparities_;
  }

  Image<uint8_t> getDisparityMap() override {
    auto minVal = min().disparity.col;
    auto maxVal = max().disparity.col;

    // this seems so inefficient
    for (auto d_it = std::begin(disparities_); d_it != std::end(disparities_); ++d_it) {
      auto i = (*d_it).leftPos.row;
      auto j = (*d_it).leftPos.col;
      auto d = (*d_it).disparity.col;

      disparityMap_(i, j) = static_cast<uint8_t>((d - minVal) * 255 / (maxVal - minVal));
    }
    return disparityMap_;
  }

  DisparityPoint max() override {
    return *(std::max(std::begin(disparities_), std::end(disparities_)));
  }

  DisparityPoint min() override {
    return *(std::min(std::begin(disparities_), std::end(disparities_)));
  }

 private:
  DisparityPoints disparities_;
  Image<uint8_t> disparityMap_;
};
}  // namespace s3d

#endif  // S3D_DISPARITY_DISPARITIES_SPARSE_H
