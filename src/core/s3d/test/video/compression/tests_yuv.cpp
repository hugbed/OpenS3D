#include "gtest/gtest.h"

#include "s3d/video/compression/yuv.h"

using s3d::compression::BGR;
using s3d::compression::UYVY;
using s3d::compression::color_conversion;
using rgb_tuple = s3d::compression::color_conversion<UYVY, BGR>::rgb_tuple;
using yuv_tuple = s3d::compression::color_conversion<UYVY, BGR>::yuv_tuple;

namespace YUV_VALUES {
yuv_tuple BLACK = {0, 128, 128};
yuv_tuple WHITE = {255, 128, 128};
yuv_tuple RED = {76, 84, 255};
yuv_tuple GREEN = {149, 43, 21};
yuv_tuple BLUE = {29, 255, 107};
}  // namespace YUV_VALUES

namespace RGB_VALUES {
rgb_tuple BLACK = {0, 0, 0};
rgb_tuple WHITE = {255, 255, 255};
rgb_tuple RED = {255, 0, 0};
rgb_tuple GREEN = {0, 255, 0};
rgb_tuple BLUE = {0, 0, 255};
}  // namespace RGB_VALUES

class yuv_to_rgb_test : public ::testing::Test {
 public:
  color_conversion<UYVY, BGR> cvt;
  uint8_t R, G, B;
};

TEST_F(yuv_to_rgb_test, yuv_rgb_black) {
  auto res = cvt.yuv_to_rgb(YUV_VALUES::BLACK);
  EXPECT_EQ(RGB_VALUES::BLACK.r, res.r);
  EXPECT_EQ(RGB_VALUES::BLACK.g, res.g);
  EXPECT_EQ(RGB_VALUES::BLACK.b, res.b);
}

TEST_F(yuv_to_rgb_test, yuv_rgb_white) {
  auto res = cvt.yuv_to_rgb(YUV_VALUES::WHITE);
  EXPECT_EQ(RGB_VALUES::WHITE.r, res.r);
  EXPECT_EQ(RGB_VALUES::WHITE.g, res.g);
  EXPECT_EQ(RGB_VALUES::WHITE.b, res.b);
}

TEST_F(yuv_to_rgb_test, yuv_rgb_red) {
  auto res = cvt.yuv_to_rgb(YUV_VALUES::RED);
  EXPECT_EQ(RGB_VALUES::RED.r, res.r);
  EXPECT_EQ(RGB_VALUES::RED.g, res.g);
  EXPECT_EQ(RGB_VALUES::RED.b, res.b);
}

TEST_F(yuv_to_rgb_test, yuv_rgb_green) {
  auto res = cvt.yuv_to_rgb(YUV_VALUES::GREEN);
  EXPECT_EQ(RGB_VALUES::GREEN.r, res.r);
  EXPECT_EQ(RGB_VALUES::GREEN.g, res.g);
  EXPECT_EQ(RGB_VALUES::GREEN.b, res.b);
}

TEST_F(yuv_to_rgb_test, yuv_rgb_blue) {
  auto res = cvt.yuv_to_rgb(YUV_VALUES::BLUE);
  EXPECT_EQ(RGB_VALUES::BLUE.r, res.r);
  EXPECT_EQ(RGB_VALUES::BLUE.g, res.g);
  EXPECT_EQ(RGB_VALUES::BLUE.b, res.b);
}

class color_conversion_uyvy_rgb_test : public ::testing::Test {
 public:
  void setOutputGoldenRGB(std::vector<rgb_tuple> rgbTuples) {
    golden.clear();
    for (auto rgbValue : rgbTuples) {
      golden.emplace_back(rgbValue.b);
      golden.emplace_back(rgbValue.g);
      golden.emplace_back(rgbValue.r);
    }
  }

  void setInputUYVY(std::vector<yuv_tuple> yuvTuples) {
    uyvyValues.clear();
    for (auto yuvValue : yuvTuples) {
      uyvyValues.emplace_back(yuvValue.u);
      uyvyValues.emplace_back(yuvValue.y);
      uyvyValues.emplace_back(yuvValue.v);
      uyvyValues.emplace_back(yuvValue.y);
    }
  }

  void checkExpectations() {
    std::vector<uint8_t> convertedValues;
    convertedValues.resize(golden.size());
    cvt(std::begin(uyvyValues), std::end(uyvyValues), std::begin(convertedValues));

    for (int i = 0; i < golden.size(); ++i) {
      EXPECT_EQ(golden[i], convertedValues[i]);
    }
  }

  color_conversion<UYVY, BGR> cvt;
  std::vector<uint8_t> golden;
  std::vector<uint8_t> uyvyValues;
};

TEST_F(color_conversion_uyvy_rgb_test, black_black) {
  setInputUYVY({YUV_VALUES::BLACK});
  setOutputGoldenRGB({RGB_VALUES::BLACK, RGB_VALUES::BLACK});
  checkExpectations();
}

TEST_F(color_conversion_uyvy_rgb_test, red_red_for_bgr_order) {
  setInputUYVY({YUV_VALUES::RED});
  setOutputGoldenRGB({RGB_VALUES::RED, RGB_VALUES::RED});
  checkExpectations();
}

TEST_F(color_conversion_uyvy_rgb_test, green_green_for_bgr_order) {
  setInputUYVY({YUV_VALUES::GREEN});
  setOutputGoldenRGB({RGB_VALUES::GREEN, RGB_VALUES::GREEN});
  checkExpectations();
}

TEST_F(color_conversion_uyvy_rgb_test, blue_blue_for_bgr_order) {
  setInputUYVY({YUV_VALUES::BLUE});
  setOutputGoldenRGB({RGB_VALUES::BLUE, RGB_VALUES::BLUE});
  checkExpectations();
}

TEST_F(color_conversion_uyvy_rgb_test, red_red_blue_blue) {
  setInputUYVY({YUV_VALUES::RED, YUV_VALUES::BLUE});
  setOutputGoldenRGB({RGB_VALUES::RED, RGB_VALUES::RED, RGB_VALUES::BLUE, RGB_VALUES::BLUE});
  checkExpectations();
}
