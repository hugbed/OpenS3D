#ifndef S3D_FEATURES_MATCH_FINDER_H
#define S3D_FEATURES_MATCH_FINDER_H

#include "s3d/image/image.h"

#include "Eigen/Dense"

namespace s3d {

class MatchFinder {
 public:
  using Position = Eigen::Vector2d;
  using Matches = std::vector<std::vector<Position>>;
  using Images = std::vector<Image<uint8_t>>;

  // match.[0][i] <-- matches --> match.[1][i]
  virtual Matches findMatches(const Images& images) = 0;
};

}  // namespace s3d

#endif  // S3D_FEATURES_MATCH_FINDER_H
