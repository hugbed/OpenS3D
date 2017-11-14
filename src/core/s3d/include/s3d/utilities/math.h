#ifndef S3D_UTILITIES_MATH_H
#define S3D_UTILITIES_MATH_H

#include <algorithm>

namespace s3d {

template <typename T>
T clamp(const T& v, const T& lo, const T& hi) {
  return std::min(std::max(v, lo), hi);
}

template <class T>
class MovingAverage {
 public:
  MovingAverage() : value_{}, smoothingFactor_{1.0} {}
  MovingAverage(T value, T smoothingFactor) : value_{value}, smoothingFactor_{smoothingFactor} {}

  const T& addToAverage(const T& newValue) {
    value_ -= value_ / smoothingFactor_;
    value_ += newValue / smoothingFactor_;
    return value_;
  }

  operator T() const { return value_; }

  const T& getSmoothingFactor() const { return smoothingFactor_; }

  void setSmoothingFactor(T t) { smoothingFactor_ = t; }

 private:
  T value_;
  T smoothingFactor_;
};

}  // namespace s3d

#endif  // S3D_UTILITIES_MATH_H
