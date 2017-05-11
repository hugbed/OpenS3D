#ifndef S3D_MULTIVIEW_SAMPSON_DISTANCE_FUNCTION_H
#define S3D_MULTIVIEW_SAMPSON_DISTANCE_FUNCTION_H

#include "s3d/multiview/stan_fundamental_matrix_solver.h"

namespace s3d {

// Distance that can be applied to the model
class SampsonDistanceFunction {
 public:
  using PointsType = StanFundamentalMatrixSolver::PointsType;
  using ModelType = StanFundamentalMatrixSolver::ModelType;

  static void ComputeDistance(const std::vector<PointsType>& pts1,
                              const std::vector<PointsType>& pts2,
                              const ModelType& model,
                              std::vector<double>* distances);
};

}  // namespace s3d

#endif  // S3D_MULTIVIEW_SAMPSON_DISTANCE_FUNCTION_H
