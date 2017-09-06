#ifndef S3D_UTILITIES_HISTOGRAM_H
#define S3D_UTILITIES_HISTOGRAM_H

#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>

namespace s3d {

template <class T>
class Histogram {
 public:
  static Histogram Compute(const std::vector<T>& values, int nbBins);
  static int BinIdx(T value, T min, T binSize);
  static T BinSize(T min, T max, T nbBins);
  static T Min(const std::vector<T>& values);
  static T Max(const std::vector<T>& values);

  std::vector<int> counts;
  T binSize;
  int nbBins;
  T minValue;
  T maxValue;
};

// static
template <class T>
Histogram<T> Histogram<T>::Compute(const std::vector<T>& values, int nbBins) {
  auto max = Max(values);
  auto min = Min(values);

  // epsilon so that no value is exactly on max
  // to prevent binIdx == counts.size()
  constexpr auto epsilon = 0.0001;
  auto binSize = BinSize(min, max + epsilon, nbBins);

  std::vector<int> counts;
  counts.resize(static_cast<size_t>(nbBins));
  for (auto value : values) {
    auto binIdx = BinIdx(value, min, binSize);
    assert(binIdx < counts.size());
    counts[binIdx] += 1;
  }

  return {counts, binSize, nbBins, min, max};
}

// static
template <class T>
int Histogram<T>::BinIdx(T value, T min, T binSize) {
  return static_cast<int>(std::floor((value - min) / binSize));
}

// static
template <class T>
T Histogram<T>::BinSize(T min, T max, T nbBins) {
  return (max - min) / nbBins;
}

// static
template <class T>
T Histogram<T>::Min(const std::vector<T>& values) {
  return *std::min_element(std::begin(values), std::end(values));
}

// static
template <class T>
T Histogram<T>::Max(const std::vector<T>& values) {
  return *std::max_element(std::begin(values), std::end(values));
}

}  // namespace s3d

#endif  // S3D_UTILITIES_HISTOGRAM_H
