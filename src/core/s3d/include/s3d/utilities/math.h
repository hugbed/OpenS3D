//
// Created by jon on 04/04/17.
//

#ifndef PROJECT_MATH_H
#define PROJECT_MATH_H

#include <algorithm>

namespace s3d {
namespace math {

template<typename T>
T clamp(const T &v, const T &lo, const T &hi) {
  return std::min(std::max(v, lo), hi);
}

int roundUp(int numToRound, int multiple)
{
  if (multiple == 0)
    return numToRound;

  int remainder = numToRound % multiple;
  if (remainder == 0)
    return numToRound;

  return numToRound + multiple - remainder;
}

} // math
} // disparity

#endif //PROJECT_MATH_H
