#ifndef S3D_UTILITIES_CONTAINERS_H
#define S3D_UTILITIES_CONTAINERS_H

#include <algorithm>
#include <cassert>
#include <ostream>
#include <vector>

namespace s3d {

template <class Container, class T>
bool contains(Container c, T t) {
  return (std::find(std::begin(c), std::end(c), t) != std::end(c));
}

template <class InIt, class OutIt>
void copy_if_true(InIt srcBegin, InIt srcEnd, OutIt dstBegin, const std::vector<bool>& flags) {
  auto i = 0;
  std::copy_if(
      srcBegin, srcEnd, dstBegin, [&i, flags](typename InIt::value_type) { return flags[i++]; });
}

template <class T, class IndexType>
std::vector<T> values_from_indices(std::vector<T> values, std::vector<IndexType> indices) {
  std::vector<T> sample;
  sample.reserve(indices.size());

  for (auto i : indices) {
    assert(i < static_cast<IndexType>(values.size()));
    sample.emplace_back(values[i]);
  }

  return sample;
};

template <class InIt, class OutIt>
void center_values(InIt first, InIt last, OutIt result, typename InIt::value_type center) {
  std::transform(first, last, result, [&center](const typename InIt::value_type& value) {
    return value - center;
  });
};

template <class InIt, class Delimiter>
void display_values(InIt first, InIt last, std::ostream& os, Delimiter delimiter = ',') {
  for (auto it = first; it != last; ++it) {
    os << *it;
    if (it + 1 != last) {
      os << delimiter;
    }
  }
}

}  // namespace s3d

#endif  // S3D_UTILITIES_CONTAINERS_H
