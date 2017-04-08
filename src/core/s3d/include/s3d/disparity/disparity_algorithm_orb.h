//
// Created by jon on 06/03/17.
//

#ifndef S3D_DISPARITY_DISPARITY_ALGORITHM_ORB_H
#define S3D_DISPARITY_DISPARITY_ALGORITHM_ORB_H

#include "disparity_algorithm.h"
#include "s3d/utilities/time.h"

#include <iostream>
#include <memory>

namespace s3d {

class DisparityAlgorithmORB : public DisparityAlgorithm {
 public:
  std::unique_ptr<Disparities> ComputeDisparities(Image leftImg,
                                                  Image rightImg) override;
};
}

#endif  // S3D_DISPARITY_DISPARITY_ALGORITHM_ORB_H
