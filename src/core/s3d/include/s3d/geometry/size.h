#ifndef S3D_GEOMETRY_SIZE_H
#define S3D_GEOMETRY_SIZE_H

#include <stdexcept>
#include <tuple>

namespace s3d {

struct negative_size_exception : public std::invalid_argument {
  negative_size_exception() : std::invalid_argument{"Negative size"} {}
};

struct Size {
 public:
  // integer for relative index use
  using value_type = int;

  constexpr Size() noexcept = default;
  constexpr Size(value_type width, value_type height)
      : width_(width >= 0 ? width : throw negative_size_exception{}),
        height_(height >= 0 ? height : throw negative_size_exception{}) {}

  constexpr value_type getWidth() const noexcept { return width_; }
  constexpr value_type getHeight() const noexcept { return height_; }

  void setWidth(value_type width) { width_ = width >= 0 ? width : throw negative_size_exception{}; }
  void setHeight(value_type height) {
    height_ = height >= 0 ? height : throw negative_size_exception{};
  }

  constexpr int getArea() const noexcept { return width_ * height_; }

  constexpr bool operator==(const Size& other) const {
    return std::make_tuple(width_, height_) == std::make_tuple(other.width_, other.height_);
  }

  constexpr bool operator!=(const Size& other) const { return !(*this == other); }

 private:
  value_type width_{0};
  value_type height_{0};
};

}  // namespace s3d

#endif  // S3D_GEOMETRY_SIZE_H
