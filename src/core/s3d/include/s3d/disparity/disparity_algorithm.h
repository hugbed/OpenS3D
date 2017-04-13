//
// Created by jon on 06/03/17.
//

#ifndef S3D_DISPARITY_DISPARITY_ALGORITHM_H
#define S3D_DISPARITY_DISPARITY_ALGORITHM_H

#include "disparities.h"

#include <memory>

namespace s3d {

class DisparityAlgorithm {
 public:
  virtual std::unique_ptr<Disparities> ComputeDisparities(Image left, Image right) = 0;
};
}  // namespace s3d

#endif  // S3D_DISPARITY_DISPARITY_ALGORITHM_H
