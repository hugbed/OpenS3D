#ifndef S3D_UTILITIES_STATS_H
#define S3D_UTILITIES_STATS_H

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

}  // namespace s3d

#endif  // S3D_UTILITIES_STATS_H
