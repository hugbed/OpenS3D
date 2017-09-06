#ifndef S3D_UTILITIES_RAND_H
#define S3D_UTILITIES_RAND_H

#include "s3d/utilities/containers.h"

#include <algorithm>
#include <random>
#include <vector>

#include <cassert>

struct NumberGenerator {
  static std::mt19937 mt;
};

namespace s3d {

// todo: naive implementation, could take infinite time
// better implementation would be to shuffle then choose n first
template <class SizeType>
std::vector<int> rand_n_unique_values(int minVal, int maxVal, SizeType n, int seed) {
  assert(minVal < maxVal);

//  std::mt19937 mt(seed);
  std::uniform_int_distribution<int> dist(minVal, maxVal);

  std::vector<int> randValues;
  randValues.reserve(n);

  for (auto i = 0ULL; i < n; ++i) {
    auto randNb = dist(NumberGenerator::mt);
    while (s3d::contains(randValues, randNb)) {
      randNb = dist(NumberGenerator::mt);
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
