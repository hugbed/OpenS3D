// Copyright (c) 2012, 2013 Pierre MOULON.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// Inspired by openMVG Image class

#ifndef S3D_IMAGE_IMAGE_CONTAINER_H
#define S3D_IMAGE_IMAGE_CONTAINER_H

#include "s3d/geometry/size.h"

#include "Eigen/Dense"

namespace s3d {

template <typename T>
class Image : public Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> {
 public:
  using value_type = T;

  using Base = Eigen::Matrix<value_type, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

  Image() { Base::resize(0, 0); }

  explicit Image(const Base& other) : Base(other) {}

  explicit Image(Base&& other) : Base(std::move(other)) {}

  explicit Image(Size size) { Base::resize(size.getHeight(), size.getWidth()); }

  Image(Size size, const value_type& fillValue) {
    Base::resize(size.getHeight(), size.getWidth());
    Base::fill(fillValue);
  }

  int width() const { return static_cast<int>(Base::cols()); }
  int height() const { return static_cast<int>(Base::rows()); }
  int depth() const { return sizeof(value_type); }

  const value_type& operator()(int y, int x) const { return Base::operator()(y, x); }

  value_type& operator()(int y, int x) { return Base::operator()(y, x); }

  const value_type& operator[](int p) const { return Base::operator()(p); }

  value_type& operator[](int p) { return Base::operator()(p); }

  void resize(Size size) { Base::resize(size.getHeight(), size.getWidth()); }
  void resize(Size size, const value_type& fillValue) {
    Base::resize(size.getHeight(), size.getWidth());
    Base::fill(fillValue);
  }

  const Base& getMatrix() const { return (*this); }
  Image& setMatrix(const Base& I) {
    Base::operator=(I);
    return *this;
  }
};

}  // namespace s3d

#endif  // S3D_IMAGE_IMAGE_CONTAINER_H
