#ifndef S3D_VIDEO_COMPRESSION_YUV_H
#define S3D_VIDEO_COMPRESSION_YUV_H

#include "s3d/utilities/math.h"
#include <tuple>

namespace s3d {
namespace compression {

class YUV422 {};
class RGB8 {};

template <class InType, class OutType>
class color_conversion {
  //  template <class InputIt, class OutputIt>
  //  void operator()(InputIt first, InputIt last, OutputIt d_first) {}
};

template <>
struct color_conversion<YUV422, RGB8> {
  static std::tuple<uint8_t, uint8_t, uint8_t> yuv_to_rgb(uint8_t Y,
                                                          uint8_t U,
                                                          uint8_t V) {
    static_assert(std::is_same<uint8_t, std::uint8_t>::value,
                  "only byte sequences are supported");

    int C = Y - 16;
    int D = U - 128;
    int E = V - 128;

    using s3d::math::clamp;
    auto R =
        static_cast<uint8_t>(clamp((298 * C + 409 * E + 128) >> 8, 0, 255));
    auto G = static_cast<uint8_t>(
        clamp((298 * C - 100 * D - 208 * E + 128) >> 8, 0, 255));
    auto B =
        static_cast<uint8_t>(clamp((298 * C + 516 * D + 128) >> 8, 0, 255));

    return std::tuple<uint8_t, uint8_t, uint8_t>{R, G, B};
  }

  template <class InputIt, class OutputIt>
  void operator()(InputIt first, InputIt last, OutputIt d_first) {
    using std::is_same;
    using std::distance;
    using std::iterator_traits;
    static_assert(
        is_same<typename iterator_traits<InputIt>::value_type, uint8_t>::value,
        "only byte sequences are supported");
    //  assert(distance(first, last) % 4 == 0);

    uint8_t R, G, B;
    for (; first != last; first += 4) {
      auto y1 = *(first + 3);
      auto cr0 = *(first + 2);
      auto y0 = *(first + 1);
      auto cb0 = *first;

      std::tie(R, G, B) = yuv_to_rgb(y0, cb0, cr0);
      *d_first++ = B;
      *d_first++ = G;
      *d_first++ = R;

      std::tie(R, G, B) = yuv_to_rgb(y1, cb0, cr0);
      *d_first++ = B;
      *d_first++ = G;
      *d_first++ = R;
    }
  }
};

}  // namespace compression
}  // namespace s3d

#endif  // S3D_VIDEO_COMPRESSION_YUV_H