#ifndef S3D_RANDOM_COLOR_GENERATOR_H
#define S3D_RANDOM_COLOR_GENERATOR_H

#include "s3d/cv/utilities/cv.h"

#include <random>

template <class RGB>
class RandomColorGenerator {
 public:
  RandomColorGenerator() : mt(std::random_device{}()), dist(0, 255) {}

  RGB randomColor() { return RGB(dist(mt), dist(mt), dist(mt)); }

  std::mt19937 mt;
  std::uniform_int_distribution<int> dist;
};

#endif  // S3D_RANDOM_COLOR_GENERATOR_H
