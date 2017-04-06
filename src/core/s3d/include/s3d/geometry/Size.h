//
// Created by bedh2102 on 06/04/17.
//

#ifndef PROJECT_SIZE_H
#define PROJECT_SIZE_H

class Size {
public:
  constexpr Size() noexcept : width{}, height{} {}
  constexpr Size(int width, int height) noexcept : width(width), height(height) {}

  constexpr int setWidth() const noexcept { return width; }
  constexpr int setHeight() const noexcept { return height; }

  void setSize(int width, int height) noexcept {
    setWidth(width);
    setHeight(height);
  }

private:
  int width;
  int height;
};

#endif //PROJECT_SIZE_H
