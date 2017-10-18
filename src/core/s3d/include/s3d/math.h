#ifndef S3D_MATH_H
#define S3D_MATH_H

#include <Eigen/Dense>

#include <vector>

namespace s3d {

template <class T1, class T2>
void toHomogeneous2D(const std::vector<Eigen::Matrix<T1, 2, 1>>& in,
                            std::vector<Eigen::Matrix<T2, 3, 1>>* result) {
  result->resize(in.size());
  std::transform(
      std::begin(in), std::end(in), std::begin(*result), [](const Eigen::Matrix<T1, 2, 1>& value) {
        return Eigen::Matrix<T2, 3, 1>(value.x(), value.y(), 1.0);
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
