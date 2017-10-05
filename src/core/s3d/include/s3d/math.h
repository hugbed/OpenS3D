#ifndef S3D_MATH_H
#define S3D_MATH_H

#include <Eigen/Dense>

#include <vector>

namespace s3d {
inline void toHomogeneous2D(const std::vector<Eigen::Vector2d>& in,
                            std::vector<Eigen::Vector3d>* result) {
  result->resize(in.size());
  std::transform(
      std::begin(in), std::end(in), std::begin(*result), [](const Eigen::Vector2d& value) {
        return Eigen::Vector3d(value.x(), value.y(), 1.0);
      });
}

// usually have to divide by z() but not necessary here
inline void toEuclidian2DTruncate(const std::vector<Eigen::Vector3d>& in,
                                  std::vector<Eigen::Vector2d>* result) {
  result->resize(in.size());
  std::transform(
      std::begin(in), std::end(in), std::begin(*result), [](const Eigen::Vector3d& value) {
        return Eigen::Vector2d(value.x(), value.y());
      });
}
}  // namespace s3d

#endif  // S3D_MATH_H
