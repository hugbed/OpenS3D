#ifndef S3D_MULTIVIEW_STAN_RESULTS_H
#define S3D_MULTIVIEW_STAN_RESULTS_H

#include "s3d/multiview/stan_alignment.h"

#include <Eigen/Dense>

#include <vector>

namespace s3d {

/**
 * Includes all results from the STAN algorithm computations.
 */
struct StanResults {
  std::vector<Eigen::Vector2f> featuresLeft;
  std::vector<Eigen::Vector2f> featuresRight;
  StanAlignment alignment;
  Eigen::Matrix3f F;
  Eigen::Matrix3f HLeft;
  Eigen::Matrix3f HRight;
};

} // namespace s3d

#endif //S3D_MULTIVIEW_STAN_RESULTS_H
