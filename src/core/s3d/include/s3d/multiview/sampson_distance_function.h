#ifndef S3D_MULTIVIEW_SAMPSON_DISTANCE_FUNCTION_H
#define S3D_MULTIVIEW_SAMPSON_DISTANCE_FUNCTION_H

// std::vector requires complete type
#include <Eigen/Dense>

namespace s3d {

struct StanAlignment;

class SampsonDistanceFunction {
 public:
  using PointsType = Eigen::Vector3d;
  using ModelType = StanAlignment;

  static void ComputeDistance(const std::vector<PointsType>& pts1,
                              const std::vector<PointsType>& pts2,
                              const ModelType& model,
                              std::vector<double>* distances);
};

}  // namespace s3d

#endif  // S3D_MULTIVIEW_SAMPSON_DISTANCE_FUNCTION_H
