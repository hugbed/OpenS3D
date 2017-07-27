#include "s3d/utilities/histogram.h"

#include <cassert>

namespace s3d {

// static
Histogram Histogram::Compute(const std::vector<double>& values, int nbBins) {
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
int Histogram::BinIdx(double value, double min, double binSize) {
  return static_cast<int>(std::floor((value - min) / binSize));
}

// static
double Histogram::BinSize(double min, double max, double nbBins) {
  return (max - min) / nbBins;
}

// static
double Histogram::Min(const std::vector<double>& values) {
  return *std::min_element(std::begin(values), std::end(values));
}

// static
double Histogram::Max(const std::vector<double>& values) {
  return *std::max_element(std::begin(values), std::end(values));
}

}  // namespace s3d
