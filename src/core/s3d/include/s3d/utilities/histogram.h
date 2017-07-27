#ifndef S3D_UTILITIES_HISTOGRAM_H
#define S3D_UTILITIES_HISTOGRAM_H

#include <algorithm>
#include <cmath>
#include <vector>

namespace s3d {

class Histogram {
 public:
  static Histogram Compute(const std::vector<double>& values, int nbBins);
  static int BinIdx(double value, double min, double binSize);
  static double BinSize(double min, double max, double nbBins);
  static double Min(const std::vector<double>& values);
  static double Max(const std::vector<double>& values);

  std::vector<int> counts;
  double binSize;
  int nbBins;
  double minValue;
  double maxValue;
};

}  // namespace s3d

#endif  // S3D_UTILITIES_HISTOGRAM_H
