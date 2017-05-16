#ifndef S3DCUDA_DISPARITY_DISPARITY_ALGORITHM_BM_H
#define S3DCUDA_DISPARITY_DISPARITY_ALGORITHM_BM_H

#include "s3d/disparity/disparity_algorithm.h"

#include <memory>

namespace s3d {
namespace cuda {

class DisparityAlgorithmBM : public s3d::DisparityAlgorithm {
 public:
  std::unique_ptr<Disparities> ComputeDisparities(const Image<uint8_t>& leftImg,
                                                  const Image<uint8_t>& rightImg) override;
};

}  // namespace cuda
}  // namespace s3d

#endif  // S3DCUDA_DISPARITY_DISPARITY_ALGORITHM_BM_H
