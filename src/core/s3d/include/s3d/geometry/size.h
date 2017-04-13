//
// Created by bedh2102 on 06/04/17.
//

#ifndef S3D_GEOMETRY_SIZE_H
#define S3D_GEOMETRY_SIZE_H

// todo(hugbed): put in namespace s3d

class Size {
 public:
  constexpr Size() noexcept : width_{}, height_{} {}
  constexpr Size(int width, int height) noexcept : width_(width), height_(height) {}

  void setWidth(int width) noexcept { height_ = width; }
  void setHeight(int height) noexcept { height_ = height; }
  constexpr int getWidth() const noexcept { return width_; }
  constexpr int getHeight() const noexcept { return height_; }

  void setSize(int width, int height) noexcept {
    setWidth(width);
    setHeight(height);
  }

  int getArea() const noexcept { return width_ * height_; }

 private:
  int width_;
  int height_;
};

#endif  // S3D_GEOMETRY_SIZE_H
