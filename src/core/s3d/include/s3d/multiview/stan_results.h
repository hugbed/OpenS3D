#ifndef S3D_MULTIVIEW_STAN_RESULTS_H
#define S3D_MULTIVIEW_STAN_RESULTS_H

#include "s3d/multiview/stan_alignment.h"
#include "s3d/utilities/eigen.h"

#include <vector>

namespace s3d {

/**
 * Includes all results from the STAN algorithm computations.
 */
struct StanResults {
  StanResults() {
    // rectified essential matrix by default;
    F(1, 2) = -1;
    F(2, 1) = 1;
  }

  std::vector<Eigen::Vector2f> featuresLeft{};
  std::vector<Eigen::Vector2f> featuresRight{};
  StanAlignment alignment{};
  Eigen::Matrix3f F{Eigen::Matrix3f::Zero()};
  Eigen::Matrix3f HLeft{Eigen::Matrix3f::Identity()}; // no rectification
  Eigen::Matrix3f HRight{Eigen::Matrix3f::Identity()}; // no rectification
};

} // namespace s3d

#endif //S3D_MULTIVIEW_STAN_RESULTS_H
