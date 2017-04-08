//
// Created by jon on 04/04/17.
//

#ifndef S3D_DISPARITY_DISPARITIES_SPARSE_H
#define S3D_DISPARITY_DISPARITIES_SPARSE_H

#include "disparities.h"

#include <vector>

namespace s3d {

class DisparitiesSparse : public Disparities {
 public:
  DisparitiesSparse(const std::vector<DisparityPoint>& disparities,
                    ImageSize imageSize)
      : disparities_(disparities),
        disparityMap_(cv::Mat::zeros(static_cast<int>(imageSize.rows),
                                     static_cast<int>(imageSize.cols),
                                     CV_8U)) {}

  virtual const std::vector<DisparityPoint>& getDisparities()
      override  // const & ??
  {
    return disparities_;
  }

  virtual Image getDisparityMap() override {
    auto minVal = min().disparity.col;
    auto maxVal = max().disparity.col;

    // this seems so inefficient
    for (auto d_it = std::begin(disparities_); d_it != std::end(disparities_);
         ++d_it) {
      auto i = (*d_it).leftPos.row;
      auto j = (*d_it).leftPos.col;
      auto d = (*d_it).disparity.col;

      disparityMap_.mat.at<uchar>(i, j) =
          static_cast<uchar>((d - minVal) * 255 / (maxVal - minVal));
    }
    return disparityMap_;
  }

  virtual DisparityPoint max() override {
    return *(std::max(std::begin(disparities_), std::end(disparities_)));
  }

  virtual DisparityPoint min() override {
    return *(std::min(std::begin(disparities_), std::end(disparities_)));
  }

 private:
  std::vector<DisparityPoint> disparities_;
  Image disparityMap_;
};
}

#endif  // S3D_DISPARITY_DISPARITIES_SPARSE_H
