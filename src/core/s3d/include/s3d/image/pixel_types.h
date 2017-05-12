// Copyright (c) 2012, 2013 Pierre MOULON.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// Inspired by openMVG Image class

#include <Eigen/Dense>

#include <cstdint>  // uint8_t

namespace s3d {

template <typename T>
class RgbBase : public Eigen::Matrix<T, 3, 1, 0, 3, 1> {
  // Full internal type
  using Base = Eigen::Matrix<T, 3, 1, 0, 3, 1>;

  // Color component type
  using TBase = T;

 public:
  RgbBase(T red, T green, T blue) : Base(red, green, blue) {}
  explicit inline RgbBase(const Base& val) : Base(val) {}
  explicit inline RgbBase(const T& val = 0) : Base(val, val, val) {}

  const T& r() const { return (*this)(0); }
  T& r() { return (*this)(0); }

  const T& g() const { return (*this)(1); }
  T& g() { return (*this)(1); }

  const T& b() const { return (*this)(2); }
  T& b() { return (*this)(2); }

  // Get approximate Gray value using rec601 Luma conversion
  explicit operator T() const { return T(0.299 * r() + 0.587 * g() + 0.114 * b()); }

  friend std::ostream& operator<<(std::ostream& os, const RgbBase& col) {
    os << "{";
    for (int i = 0; i < 2; ++i) {
      os << static_cast<int>(col(i)) << ",";
    }
    os << static_cast<int>(col(2)) << "}";
    return os;
  }
};

// Instantiation for unsigned char color component
using RGBColor = RgbBase<uint8_t>;

namespace RGBValues {
static const RGBColor WHITE(255, 255, 255);
static const RGBColor BLACK(0, 0, 0);
static const RGBColor BLUE(0, 0, 255);
static const RGBColor RED(255, 0, 0);
static const RGBColor GREEN(0, 255, 0);
static const RGBColor YELLOW(255, 255, 0);
static const RGBColor CYAN(0, 255, 255);
static const RGBColor MAGENTA(255, 0, 255);
}
}
