#ifndef S3D_RANDOM_COLOR_GENERATOR_H
#define S3D_RANDOM_COLOR_GENERATOR_H

#include "s3d/cv/utilities/cv.h"

#include <random>

namespace s3d {

template <class RGB>
class RandomColorGenerator {
 public:
  RandomColorGenerator() : mt(0), dist(0, 255) {}

  RGB randomColor() { return RGB(dist(mt), dist(mt), dist(mt)); }

  std::mt19937 mt;
  std::uniform_int_distribution<int> dist;
};

}  // namespace s3d

#endif  // S3D_RANDOM_COLOR_GENERATOR_H
