#ifndef S3D_DISPARITY_DISPARITY_ANALYZER_H
#define S3D_DISPARITY_DISPARITY_ANALYZER_H

#include "s3d/image/image_container.h"
#include "s3d/utilities/rule_of_five.h"

#include <vector>

namespace s3d {

// todo: this is DisparityAnalyzerSparse, could have a DisparityAnalyzerDense
class DisparityAnalyzer : rule_of_five_interface<DisparityAnalyzer> {
  virtual bool analyze(const Image<uint8_t>& leftImage, const Image<uint8_t>& rightImage) = 0;
  virtual const std::vector<float>& getDisparitiesPercent() const = 0;
  virtual const std::vector<Eigen::Vector2f>& getFeaturePoints() const = 0;
};
}

#endif  // S3D_DISPARITY_DISPARITY_ANALYZER_H
