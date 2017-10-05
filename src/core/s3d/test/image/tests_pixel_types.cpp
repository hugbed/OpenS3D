#include "gtest/gtest.h"

#include "s3d/image/pixel_types.h"

using s3d::RgbBase;
using s3d::RgbColor;

TEST(rgb_base, default_constructor_black) {
  RgbBase<uint8_t> c;
  EXPECT_EQ(c.r(), 0);
  EXPECT_EQ(c.g(), 0);
  EXPECT_EQ(c.b(), 0);
}

TEST(rgb_color, default_constructor_black) {
  RgbColor c;
  EXPECT_EQ(c.r(), 0);
  EXPECT_EQ(c.g(), 0);
  EXPECT_EQ(c.b(), 0);
}

TEST(rgb_color, params_constructor_correct_values) {
  RgbColor c(1, 2, 3);
  EXPECT_EQ(c.r(), 1);
  EXPECT_EQ(c.g(), 2);
  EXPECT_EQ(c.b(), 3);
}

TEST(rgb_color, single_param_constructor_correct_values) {
  RgbColor c(1);
  EXPECT_EQ(c.r(), 1);
  EXPECT_EQ(c.g(), 1);
  EXPECT_EQ(c.b(), 1);
}

TEST(rgb_color, construction_from_eigen) {
  // Eigen::Matrix<Type, Rows, Cols, Options, MaxRows, MaxCols>
  Eigen::Matrix<uint8_t, 3, 1, 0, 3, 1> v(1, 2, 3);

  RgbColor c(v);
  EXPECT_EQ(c.r(), 1);
  EXPECT_EQ(c.g(), 2);
  EXPECT_EQ(c.b(), 3);
}

TEST(rgb_color, grayscale_operator) {
  RgbColor c(100, 128, 32);
  uint8_t grayValue = static_cast<uint8_t>(c);
  EXPECT_EQ(grayValue, 108);
}

TEST(rgb_color, ostream_operator) {
  std::ostringstream stream;
  stream << RgbColor(66, 66, 66);
  EXPECT_EQ(stream.str(), std::string("{66,66,66}"));
}

TEST(rgb_color, constants_are_correct) {
  RgbColor c;

  EXPECT_EQ(RgbColor(255, 255, 255), s3d::RgbValues::WHITE);
  EXPECT_EQ(RgbColor(0, 0, 0), s3d::RgbValues::BLACK);
  EXPECT_EQ(RgbColor(0, 0, 255), s3d::RgbValues::BLUE);
  EXPECT_EQ(RgbColor(255, 0, 0), s3d::RgbValues::RED);
  EXPECT_EQ(RgbColor(0, 255, 0), s3d::RgbValues::GREEN);
  EXPECT_EQ(RgbColor(255, 255, 0), s3d::RgbValues::YELLOW);
  EXPECT_EQ(RgbColor(0, 255, 255), s3d::RgbValues::CYAN);
  EXPECT_EQ(RgbColor(255, 0, 255), s3d::RgbValues::MAGENTA);
}
