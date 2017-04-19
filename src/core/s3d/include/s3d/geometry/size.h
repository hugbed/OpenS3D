//
// Created by bedh2102 on 06/04/17.
//

#ifndef S3D_GEOMETRY_SIZE_H
#define S3D_GEOMETRY_SIZE_H

// todo(hugbed): put in namespace s3d

class Size {
 public:
  using value_type = int;

  constexpr Size() noexcept : width_{}, height_{} {}
  constexpr Size(value_type width, value_type height) noexcept : width_(width), height_(height) {}

  void setWidth(value_type width) noexcept { height_ = width; }
  void setHeight(value_type height) noexcept { height_ = height; }
  constexpr value_type getWidth() const noexcept { return width_; }
  constexpr value_type getHeight() const noexcept { return height_; }

  void setSize(value_type width, value_type height) noexcept {
    setWidth(width);
    setHeight(height);
  }

  int getArea() const noexcept { return width_ * height_; }

 private:
  value_type width_;
  value_type height_;
};

#endif  // S3D_GEOMETRY_SIZE_H
