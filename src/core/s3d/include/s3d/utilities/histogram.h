#ifndef S3D_UTILITIES_HISTOGRAM_H
#define S3D_UTILITIES_HISTOGRAM_H

#include <algorithm>
#include <cmath>
#include <vector>

class Histogram {
 public:
  static Histogram Compute(const std::vector<double>& values, int nbBins) {
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

  static int BinIdx(double value, double min, double binSize) {
    return static_cast<int>(std::floor((value - min) / binSize));
  }

  static double BinSize(double min, double max, double nbBins) { return (max - min) / nbBins; }

  static double Min(const std::vector<double>& values) {
    return *std::min_element(std::begin(values), std::end(values));
  }

  static double Max(const std::vector<double>& values) {
    return *std::max_element(std::begin(values), std::end(values));
  }

  std::vector<int> counts;
  double binSize;
  int nbBins;
  double minValue;
  double maxValue;
};

#endif  // S3D_UTILITIES_HISTOGRAM_H
