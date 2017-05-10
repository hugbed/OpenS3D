#ifndef S3D_UTILITIES_CONTAINERS_H
#define S3D_UTILITIES_CONTAINERS_H

#include <algorithm>
#include <vector>
#include <cassert>

namespace s3d {

template <class Container, class T>
bool contains(Container c, T t) {
  return (std::find(std::begin(c), std::end(c), t) != std::end(c));
}

template <class InIt, class OutIt>
void copy_if_true(InIt srcBegin, InIt srcEnd, OutIt dstBegin, const std::vector<bool>& flags) {
  auto i = 0;
  std::copy_if(srcBegin, srcEnd, dstBegin,
               [&i, flags](typename InIt::value_type) { return flags[i++]; });
}

template <class T>
std::vector<T> values_from_indices(std::vector<T> values, std::vector<int> indices) {
  std::vector<T> sample;
  sample.reserve(indices.size());

  for (auto i : indices) {
    assert(i < values.size());
    sample.emplace_back(values[i]);
  }

  return sample;
};

}  // namespace s3d

#endif  // S3D_UTILITIES_CONTAINERS_H
