#ifndef S3D_VIDEO_COMPRESSION_YUV_H
#define S3D_VIDEO_COMPRESSION_YUV_H

#include "s3d/utilities/math.h"
#include <tuple>

namespace s3d {
namespace compression {

class UYVY {};
class BGR {};

template <class InType, class OutType>
class color_conversion {
  //  template <class InputIt, class OutputIt>
  //  void operator()(InputIt first, InputIt last, OutputIt d_first) {}
};

template <>
struct color_conversion<UYVY, BGR> {
  struct rgb_tuple {
    uint8_t r;
    uint8_t g;
    uint8_t b;
  };

  struct yuv_tuple {
    uint8_t y;
    uint8_t u;
    uint8_t v;
  };

  static rgb_tuple yuv_to_rgb(yuv_tuple yuv) {
    static_assert(std::is_same<uint8_t, std::uint8_t>::value, "only byte sequences are supported");

    int C = static_cast<int>(yuv.y) - 16;
    int D = static_cast<int>(yuv.u) - 128;
    int E = static_cast<int>(yuv.v) - 128;

    using s3d::math::clamp;
    auto R = static_cast<uint8_t>(clamp((298 * C + 409 * E + 128) >> 8, 0, 255));
    auto G = static_cast<uint8_t>(clamp((298 * C - 100 * D - 208 * E + 128) >> 8, 0, 255));
    auto B = static_cast<uint8_t>(clamp((298 * C + 516 * D + 128) >> 8, 0, 255));

    return {R, G, B};
  }

  template <class InputIt, class OutputIt>
  void operator()(InputIt first, InputIt last, OutputIt d_first) {
    using std::is_same;
    using std::distance;
    using std::iterator_traits;
    static_assert(is_same<typename iterator_traits<InputIt>::value_type, uint8_t>::value,
                  "only byte sequences are supported");
    //  assert(distance(first, last) % 4 == 0);

    uint8_t R, G, B;
    for (; first != last; first += 4) {
      auto y1 = *(first + 3);
      auto cr0 = *(first + 2);
      auto y0 = *(first + 1);
      auto cb0 = *first;

      auto rgbValue = yuv_to_rgb({y0, cb0, cr0});
      *d_first++ = rgbValue.b;
      *d_first++ = rgbValue.g;
      *d_first++ = rgbValue.r;

      rgbValue = yuv_to_rgb({y1, cb0, cr0});
      *d_first++ = rgbValue.b;
      *d_first++ = rgbValue.g;
      *d_first++ = rgbValue.r;
    }
  }
};

}  // namespace compression
}  // namespace s3d

#endif  // S3D_VIDEO_COMPRESSION_YUV_H
