#include "gtest/gtest.h"

#include "s3d/image/image.h"

using s3d::Image;
using s3d::RgbColor;
using s3d::Size;

TEST(image_container, default_construction_size_0) {
  Image<uint8_t> i;
  EXPECT_EQ(i.width(), 0);
  EXPECT_EQ(i.height(), 0);
}

TEST(image_container, copy_construction_from_eigen_image_row_major) {
  // Eigen::Matrix<Type, Rows, Cols, Options, MaxRows, MaxCols>
  constexpr size_t N = 2;
  Eigen::Matrix<uint8_t, N, N, 0, N, N> m;
  m << 1, 2,  //
      3, 4;

  // todo: this actually calls move constructor
  const auto& mRef = m;
  Image<uint8_t> i(mRef);

  EXPECT_EQ(i.width(), mRef.cols());
  EXPECT_EQ(i.height(), mRef.rows());
  EXPECT_EQ(i[0], mRef(0));
  EXPECT_EQ(i[1], mRef(2));
  EXPECT_EQ(i[2], mRef(1));
  EXPECT_EQ(i[3], mRef(3));
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
  Image<RgbColor> i;
  EXPECT_EQ(i.depth(), 3);
}

TEST(image_container, pixel_assignment_and_accessors_row_major) {
  Image<uint8_t> i(Size(2, 2));
  i(0, 0) = 0x00;
  i(0, 1) = 0x01;
  i(1, 0) = 0x02;
  i(1, 1) = 0x03;

  EXPECT_EQ(i[0], 0x00);
  EXPECT_EQ(i[1], 0x01);
  EXPECT_EQ(i[2], 0x02);
  EXPECT_EQ(i[3], 0x03);

  // must be row major
  EXPECT_EQ(i[0], 0x00);
  EXPECT_EQ(i[1], 0x01);
  EXPECT_EQ(i[2], 0x02);
  EXPECT_EQ(i[3], 0x03);
}

TEST(image_container, index_assignment_pixel_accessor) {
  Image<uint8_t> i(Size(2, 2));
  i[0] = 0x00;
  i[1] = 0x01;
  i[2] = 0x02;
  i[3] = 0x03;

  EXPECT_EQ(i(0, 0), 0);
  EXPECT_EQ(i(0, 1), 1);
  EXPECT_EQ(i(1, 0), 2);
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

struct Idx {
  int i;
  int j;
};

void verifyConstSquareAccessor(const Image<uint8_t>& img, std::vector<uint8_t> gold) {
  for (int i = 0; i < gold.size(); ++i) {
    EXPECT_EQ(img[i], gold[i]);
  }
}

void verifyConstPixelAccessor(const Image<uint8_t>& img,
                              std::vector<uint8_t> gold,
                              std::vector<Idx> indices) {
  for (int i = 0; i < gold.size(); ++i) {
    auto idx = indices[i];
    EXPECT_EQ(img(idx.i, idx.j), gold[i]);
  }
}

TEST(image_container, const_accessors) {
  std::vector<uint8_t> gold{1, 2, 3, 4};
  Image<uint8_t> i(Size(2, 2));
  i << gold[0], gold[1],  //
      gold[2], gold[3];
  verifyConstSquareAccessor(i, gold);
  verifyConstPixelAccessor(i, gold, {{0, 0}, {0, 1}, {1, 0}, {1, 1}});
}
