#include "gtest/gtest.h"

#include "s3d/image/pixel_types.h"

using s3d::RGBColor;

TEST(rgb_color, default_constructor_black) {
  RGBColor c;
  EXPECT_EQ(c.r(), 0);
  EXPECT_EQ(c.g(), 0);
  EXPECT_EQ(c.b(), 0);
}

TEST(rgb_color, params_constructor_correct_values) {
  RGBColor c(1, 2, 3);
  EXPECT_EQ(c.r(), 1);
  EXPECT_EQ(c.g(), 2);
  EXPECT_EQ(c.b(), 3);
}

TEST(rgb_color, single_param_constructor_correct_values) {
  RGBColor c(1);
  EXPECT_EQ(c.r(), 1);
  EXPECT_EQ(c.g(), 1);
  EXPECT_EQ(c.b(), 1);
}

TEST(rgb_color, construction_from_eigen) {
  // Eigen::Matrix<Type, Rows, Cols, Options, MaxRows, MaxCols>
  Eigen::Matrix<uint8_t, 3, 1, 0, 3, 1> v(1, 2, 3);

  RGBColor c(v);
  EXPECT_EQ(c.r(), 1);
  EXPECT_EQ(c.g(), 2);
  EXPECT_EQ(c.b(), 3);
}

TEST(rgb_color, grayscale_operator) {
  RGBColor c(100, 128, 32);
  uint8_t grayValue = static_cast<uint8_t>(c);
  EXPECT_EQ(grayValue, 108);
}

TEST(rgb_color, ostream_operator) {
  std::ostringstream stream;
  stream << RGBColor(66, 66, 66);
  EXPECT_EQ(stream.str(), std::string("{66,66,66}"));
}

TEST(rgb_color, constants_are_correct) {
  RGBColor c;

  EXPECT_EQ(RGBColor(255, 255, 255), s3d::RGBValues::WHITE);
  EXPECT_EQ(RGBColor(0, 0, 0), s3d::RGBValues::BLACK);
  EXPECT_EQ(RGBColor(0, 0, 255), s3d::RGBValues::BLUE);
  EXPECT_EQ(RGBColor(255, 0, 0), s3d::RGBValues::RED);
  EXPECT_EQ(RGBColor(0, 255, 0), s3d::RGBValues::GREEN);
  EXPECT_EQ(RGBColor(255, 255, 0), s3d::RGBValues::YELLOW);
  EXPECT_EQ(RGBColor(0, 255, 255), s3d::RGBValues::CYAN);
  EXPECT_EQ(RGBColor(255, 0, 255), s3d::RGBValues::MAGENTA);
}
