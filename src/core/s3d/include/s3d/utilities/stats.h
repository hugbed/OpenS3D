#ifndef S3D_UTILITIES_STATS_H
#define S3D_UTILITIES_STATS_H

#include <algorithm>
#include <cassert>
#include <cmath>

namespace s3d {

// percentile in range [0.0f, 1.0f]
//
template <class T>
T percentile(const std::vector<T>& values, float percentile) {
  assert(percentile <= 1.0f);
  assert(percentile >= 0.0f);

  auto valuesCopy = values;  // todo : maybe rethink this quick change
  std::sort(std::begin(valuesCopy), std::end(valuesCopy));

  auto N = static_cast<float>(valuesCopy.size());
  float x = percentile * (N - 1.0f) + 1.0f;
  float integral = std::floor(x);
  float remainder = x - std::floor(x);

  int index = static_cast<int>(integral);

  constexpr auto PERCENTILE_EPS = 0.00001f;
  if (remainder < PERCENTILE_EPS) {
    return valuesCopy[index - 1];
  }

  return valuesCopy[index - 1] + remainder * (valuesCopy[index] - valuesCopy[index - 1]);
}


template <class T>
T median(const std::vector<T>& values) {
  assert(values.size() > 0);

  std::vector<T> copy = values;
  std::sort(std::begin(copy), std::end(copy));
  const size_t size = copy.size();

  if (size % 2 != 0) {
    return copy[size/2];
  }
  return (copy[size/2] + copy[size/2 - 1]) / static_cast<T>(2);
}

/*
 * B. P. Welford (1962).
 * "Note on a method for calculating corrected sums of squares and products".
 * Technometrics 4(3):419–420.
 */
template <class T>
class OnlineVariance {
public:
  void update(T newValue) {
    count++;
    T delta = newValue - mean;
    mean = mean + delta / static_cast<T>(count);
    T delta2 = newValue - mean;
    m2 += delta * delta2;
  }

  void reset() {
    count = 0;
    mean = {};
    m2 = {};
  }

  // Only valid if count >= 2
  T getMean() {
    return mean;
  }

  T getVariance() {
    return m2 / (static_cast<T>(count) - 1.0);
  }

private:
  int count{};
  T mean{};
  T m2{};
};

}  // namespace s3d

#endif  // S3D_UTILITIES_STATS_H
