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

} // math
} // s3d

#endif //PROJECT_MATH_H
