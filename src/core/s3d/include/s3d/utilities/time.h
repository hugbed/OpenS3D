//
// Created by jon on 18/02/17.
//

#ifndef OPENCVTESTS_CV_HB_H_H
#define OPENCVTESTS_CV_HB_H_H

#include <opencv/cv.hpp>
#include <chrono>

// Small useful tool functions

namespace s3d {
template <typename Predicate, typename... Params>
auto mesure_time(Predicate f, Params&&... params) {
  auto t1 = std::chrono::high_resolution_clock::now();
  f(std::forward<Params>(params)...);
  auto t2 = std::chrono::high_resolution_clock::now();
  return t2 - t1;
}
}

#endif  // OPENCVTESTS_CV_HB_H_H
