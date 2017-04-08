//
// Created by jon on 06/03/17.
//

#ifndef S3D_DISPARITY_DISPARITY_ALGORITHM_BM_H
#define S3D_DISPARITY_DISPARITY_ALGORITHM_BM_H

#include "disparity_algorithm.h"

namespace s3d {

class DisparityAlgorithmBM : public DisparityAlgorithm {
 public:
  std::unique_ptr<Disparities> ComputeDisparities(Image leftImg,
                                                  Image rightImg) override;
};
}

#endif  // S3D_DISPARITY_DISPARITY_ALGORITHM_BM_H
