#include "gtest/gtest.h"

#include "s3d/image/image.h"

using s3d::Image;
using s3d::RGBColor;

TEST(image_container, default_construction_size_0) {
  Image<uint8_t> i;
  EXPECT_EQ(i.width(), 0);
  EXPECT_EQ(i.height(), 0);
}

TEST(image_container, copy_construction_from_eigen_pixel_access_operator) {
  // Eigen::Matrix<Type, Rows, Cols, Options, MaxRows, MaxCols>
  constexpr size_t N = 2;
  Eigen::Matrix<uint8_t, N, N, 0, N, N> m;
  m << 1, 2,  //
      3, 4;

  Image<uint8_t> i(m);
  EXPECT_EQ(i.width(), 2);
  EXPECT_EQ(i.height(), 2);
  EXPECT_EQ(i(0, 0), 1);
  EXPECT_EQ(i(0, 1), 2);
  EXPECT_EQ(i(1, 0), 3);
  EXPECT_EQ(i(1, 1), 4);
}

TEST(image_container, move_construction_from_eigen_pixel_access_operator) {
  // Eigen::Matrix<Type, Rows, Cols, Options, MaxRows, MaxCols>
  constexpr size_t N = 1;
  Eigen::Matrix<uint8_t, N, N, 0, N, N> m;
  m << 1;

  Image<uint8_t> i(std::move(m));
  EXPECT_EQ(i.width(), 1);
  EXPECT_EQ(i.height(), 1);
  EXPECT_EQ(i(0, 0), 1);
}

TEST(image_container, size_construction_correct_size) {
  Image<uint8_t> i(Size(2, 2));
  EXPECT_EQ(i.width(), 2);
  EXPECT_EQ(i.height(), 2);
}

TEST(image_container, size_construction_value_filled_index_access_operator) {
  Image<uint8_t> i(Size(1, 1), 2);
  EXPECT_EQ(i[0], 2);
}

TEST(image_container, depth_byte) {
  Image<uint8_t> i;
  EXPECT_EQ(i.depth(), 1);
}

TEST(image_container, depth_rgb) {
  Image<RGBColor> i;
  EXPECT_EQ(i.depth(), 3);
}

TEST(image_container, pixel_assignment_and_accessors) {
  Image<uint8_t> i(Size(2, 2));
  i(0, 0) = 0x00;
  i(0, 1) = 0x01;
  i(1, 0) = 0x02;
  i(1, 1) = 0x03;

  const auto& cval0 = i[0];
  const auto& cval11 = i(1, 1);
  auto& val2 = i[2];
  auto& val01 = i(0, 1);

  EXPECT_EQ(cval0, 0x00);
  EXPECT_EQ(cval11, 0x03);
  EXPECT_EQ(val2, 0x02);
  EXPECT_EQ(val01, 0x01);
}

TEST(image_container, index_assignment_pixel_accessor) {
  Image<uint8_t> i(Size(2, 2));
  i[0] = 0x00;
  i[1] = 0x01;
  i[2] = 0x02;
  i[3] = 0x03;

  EXPECT_EQ(i(0,0), 0);
  EXPECT_EQ(i(0,1), 1);
  EXPECT_EQ(i(1,0), 2);
  EXPECT_EQ(i(1, 1), 3);
}

TEST(image_container, resize) {
  Image<uint8_t> i(Size(2, 3));
  EXPECT_EQ(i.width(), 2);
  EXPECT_EQ(i.height(), 3);

  i.resize(Size(4, 5));
  EXPECT_EQ(i.width(), 4);
  EXPECT_EQ(i.height(), 5);
}

TEST(image_container, resize_fill) {
  Image<uint8_t> i(Size(2, 3));
  EXPECT_EQ(i.width(), 2);
  EXPECT_EQ(i.height(), 3);

  i.resize(Size(1, 1), 2);
  EXPECT_EQ(i.width(), 1);
  EXPECT_EQ(i.height(), 1);
  EXPECT_EQ(i[0], 2);
}

TEST(image_container, matrix_accessor) {
  Image<uint8_t> i(Size(1, 1), 2);
  auto m = i.getMatrix();
  EXPECT_EQ(m(0, 0), 2);
}

TEST(image_container, eigen_assignment) {
  constexpr size_t N = 2;
  Eigen::Matrix<uint8_t, N, N, 0, N, N> m;
  m << 1, 2,  //
      3, 4;

  Image<uint8_t> i;
  i.setMatrix(m);
  EXPECT_EQ(i, m);
}
