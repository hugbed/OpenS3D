//
// Created by jon on 06/03/17.
//

#ifndef S3D_DISPARITY_DISPARITY_ALGORITHM_BM_H
#define S3D_DISPARITY_DISPARITY_ALGORITHM_BM_H

#include "s3d/disparity/disparity_algorithm.h"

#include <memory>

namespace s3d {
namespace cuda {

class DisparityAlgorithmBM : public s3d::DisparityAlgorithm {
 public:
  std::unique_ptr<Disparities> ComputeDisparities(Image leftImg, Image rightImg) override;
};

}  // namespace cuda
}  // namespace s3d

#endif  // S3D_DISPARITY_DISPARITY_ALGORITHM_BM_H
