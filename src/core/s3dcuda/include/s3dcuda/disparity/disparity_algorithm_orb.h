#ifndef S3DCUDA_DISPARITY_DISPARITY_ALGORITHM_ORB_H
#define S3DCUDA_DISPARITY_DISPARITY_ALGORITHM_ORB_H

#include "s3d/disparity/disparity_algorithm.h"
#include "s3d/utilities/time.h"

#include <iostream>
#include <memory>

namespace s3d {
namespace cuda {

class DisparityAlgorithmORB : public s3d::DisparityAlgorithm {
 public:
  std::unique_ptr<Disparities> ComputeDisparities(Image<uint8_t> leftImg,
                                                  Image<uint8_t> rightImg) override;
};

}  // namespace cuda
}  // namespace s3d

#endif  // S3DCUDA_DISPARITY_DISPARITY_ALGORITHM_ORB_H
