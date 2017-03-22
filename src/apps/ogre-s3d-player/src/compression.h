//
// Created by jon on 19/03/17.
//

#ifndef OGRE_SAMPLE_COMPRESSION_H
#define OGRE_SAMPLE_COMPRESSION_H

#include <vector>
#include <cassert>
#include <tuple>
#include <algorithm>

template<typename T>
T clamp(const T& v, const T& lo, const T& hi)
{
    return std::min(std::max(v, lo), hi);
}

struct Dimensions {
    size_t width;
    size_t height;
};

std::tuple<uint8_t, uint8_t, uint8_t> yuv2rgb(uint8_t Y, uint8_t U, uint8_t V)
{
    static_assert (
            std::is_same <uint8_t, std::uint8_t>::value,
            "only byte sequences are supported"
    );

    int C = Y-16;
    int D = U-128;
    int E = V-128;

    auto R = static_cast<uint8_t>(clamp((298*C+409*E+128) >> 8, 0, 255));
    auto G = static_cast<uint8_t>(clamp((298*C-100*D-208*E+128) >> 8, 0, 255));
    auto B = static_cast<uint8_t>(clamp((298*C+516*D+128) >> 8, 0, 255));

    return std::tuple<uint8_t, uint8_t, uint8_t>{R, G, B};
}

static std::vector<uint8_t> yuv2rgb(const std::vector<uint8_t>& in)
{
    uint8_t R, G, B;
    uint8_t y0, cb0, y1, cr0;
    std::vector<uint8_t> imageRGBA;
    for (auto it = std::begin(in); it != std::end(in); it += 4) {
        y1 = *(it+3);
        cr0 = *(it+2);
        y0 = *(it+1);
        cb0 = *it;

        // maybe rgb_value is more clear, this is confusing
        std::tie(R, G, B) = yuv2rgb(y0, cb0, cr0);
        imageRGBA.push_back(B);
        imageRGBA.push_back(G);
        imageRGBA.push_back(R);
        imageRGBA.push_back(255);

        std::tie(R, G, B) = yuv2rgb(y1, cb0, cr0);
        imageRGBA.push_back(B);
        imageRGBA.push_back(G);
        imageRGBA.push_back(R);
        imageRGBA.push_back(255);
    }

    return imageRGBA;
}

template<class InputIt, class OutputIt>
static void yuv2rgb(InputIt first, InputIt last, OutputIt d_first)
{
    using std::is_same;
    using std::distance;
    using std::iterator_traits;
    static_assert (
            is_same <typename iterator_traits<InputIt>::value_type, uint8_t>::value,
            "only byte sequences are supported"
    );
    assert(distance(first, last) % 4 == 0);

    uint8_t R, G, B;
    for (; first != last; first += 4) {
        auto y1 = *(first+3);
        auto cr0 = *(first+2);
        auto y0 = *(first+1);
        auto cb0 = *first;

        std::tie(R, G, B) = yuv2rgb(y0, cb0, cr0);
        *d_first++ = B;
        *d_first++ = G;
        *d_first++ = R;

        std::tie(R, G, B) = yuv2rgb(y1, cb0, cr0);
        *d_first++ = B;
        *d_first++ = G;
        *d_first++ = R;
    }
}

#endif //OGRE_SAMPLE_COMPRESSION_H
