#ifndef S3D_UTILITIES_TIME_H
#define S3D_UTILITIES_TIME_H

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

inline std::chrono::milliseconds seconds_to_ms(float seconds) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::duration<float>(seconds));
}

}  // namespace s3d

#endif  // S3D_UTILITIES_TIME_H
