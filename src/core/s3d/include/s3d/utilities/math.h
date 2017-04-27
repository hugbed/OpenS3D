#ifndef S3D_UTILITIES_MATH_H
#define S3D_UTILITIES_MATH_H

#include <algorithm>

namespace s3d {
namespace math {

template <typename T>
T clamp(const T& v, const T& lo, const T& hi) {
  return std::min(std::max(v, lo), hi);
}

}  // namespace math
}  // namespace s3d

#endif  // S3D_UTILITIES_MATH_H
