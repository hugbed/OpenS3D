#ifndef S3D_DISPARITY_DISPARITY_ALGORITHM_H
#define S3D_DISPARITY_DISPARITY_ALGORITHM_H

#include "disparities.h"

#include <memory>

namespace s3d {

class DisparityAlgorithm {
 public:
  virtual std::unique_ptr<Disparities> ComputeDisparities(const Image<uint8_t>& left,
                                                          const Image<uint8_t>& right) = 0;
};
}  // namespace s3d

#endif  // S3D_DISPARITY_DISPARITY_ALGORITHM_H
