#ifndef S3D_UTILITIES_RAND_H
#define S3D_UTILITIES_RAND_H

#include "s3d/utilities/containers.h"

#include <algorithm>
#include <random>
#include <vector>

#include <cassert>

namespace s3d {

template <class SizeType>
std::vector<int> rand_n_unique_values(int minVal, int maxVal, SizeType n, int seed) {
  assert(minVal < maxVal);

  std::mt19937 mt(seed);
  std::uniform_int_distribution<int> dist(minVal, maxVal);

  std::vector<int> randValues;
  randValues.reserve(n);

  for (auto i = 0ULL; i < n; ++i) {
    auto randNb = dist(mt);
    while (s3d::contains(randValues, randNb)) {
      randNb = dist(mt);
    }
    randValues.emplace_back(randNb);
  }
  return randValues;
};

template <class SizeType>
std::vector<int> rand_n_unique_values(int minVal, int maxVal, SizeType n) {
  std::random_device rd;
  return rand_n_unique_values(minVal, maxVal, n, rd());
};

}  // namespace s3d

#endif  // S3D_UTILITIES_RAND_H
