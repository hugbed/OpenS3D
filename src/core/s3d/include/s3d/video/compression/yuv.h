#ifndef S3D_VIDEO_COMPRESSION_YUV_H
#define S3D_VIDEO_COMPRESSION_YUV_H

#include "s3d/utilities/math.h"
#include <tuple>

namespace s3d {
namespace compression {

class UYVY {};
class BGR {};
class RGB {};

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

  template <class It>
  static std::pair<yuv_tuple, yuv_tuple> decompose_yuv_from_uyvy_it(It uyvy_it) {
    auto y1 = *(uyvy_it + 3);
    auto cr0 = *(uyvy_it + 2);
    auto y0 = *(uyvy_it + 1);
    auto cb0 = *uyvy_it;
    return {{y0, cb0, cr0}, {y1, cb0, cr0}};
  };

  static rgb_tuple yuv_to_rgb(yuv_tuple yuv) {
    static_assert(std::is_same<uint8_t, std::uint8_t>::value, "only byte sequences are supported");

    int C = static_cast<int>(yuv.y) - 16;
    int D = static_cast<int>(yuv.u) - 128;
    int E = static_cast<int>(yuv.v) - 128;

    using s3d::clamp;
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
      auto yuv_pair = decompose_yuv_from_uyvy_it(first);

      auto rgbValue = yuv_to_rgb(yuv_pair.first);
      *d_first++ = rgbValue.b;
      *d_first++ = rgbValue.g;
      *d_first++ = rgbValue.r;

      rgbValue = yuv_to_rgb(yuv_pair.second);
      *d_first++ = rgbValue.b;
      *d_first++ = rgbValue.g;
      *d_first++ = rgbValue.r;
    }
  }
};

template <>
struct color_conversion<UYVY, RGB> {
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
      auto yuv_pair = color_conversion<UYVY, BGR>::decompose_yuv_from_uyvy_it(first);

      auto rgbValue = color_conversion<UYVY, BGR>::yuv_to_rgb(yuv_pair.first);
      *d_first++ = rgbValue.r;
      *d_first++ = rgbValue.g;
      *d_first++ = rgbValue.b;

      rgbValue = color_conversion<UYVY, BGR>::yuv_to_rgb(yuv_pair.first);
      *d_first++ = rgbValue.r;
      *d_first++ = rgbValue.g;
      *d_first++ = rgbValue.b;
    }
  }
};

}  // namespace compression
}  // namespace s3d

#endif  // S3D_VIDEO_COMPRESSION_YUV_H
