#include "gtest/gtest.h"

#include "s3d/utilities/math.h"

class math_clamp_int : public ::testing::Test {
 protected:
  using value_type = int;
  void SetUp() override {
    lowBound = 5;
    highBound = 10;
  }
  value_type lowBound{};
  value_type highBound{};
};

TEST_F(math_clamp_int, smaller_gives_low_bound) {
  auto value = lowBound - 1;
  auto res = s3d::clamp(value, lowBound, highBound);
  EXPECT_EQ(res, lowBound);
}

TEST_F(math_clamp_int, low_gives_low_bound) {
  auto value = lowBound;
  auto res = s3d::clamp(value, lowBound, highBound);
  EXPECT_EQ(res, lowBound);
}

TEST_F(math_clamp_int, inside_gives_value) {
  auto value = lowBound + 1;
  auto res = s3d::clamp(value, lowBound, highBound);
  EXPECT_EQ(res, value);
}

TEST_F(math_clamp_int, high_gives_high_bound) {
  auto value = highBound;
  auto res = s3d::clamp(value, lowBound, highBound);
  EXPECT_EQ(res, highBound);
}

TEST_F(math_clamp_int, larger_gives_high_bound) {
  auto value = highBound + 1;
  auto res = s3d::clamp(value, lowBound, highBound);
  EXPECT_EQ(res, highBound);
}

class math_clamp_float : public ::testing::Test {
 protected:
  using value_type = float;
  void SetUp() override {
    lowBound = 5.0f;
    highBound = 10.0f;
  }
  value_type lowBound;
  value_type highBound;
};

TEST_F(math_clamp_float, smaller_gives_low_bound) {
  auto value = lowBound - 1.0f;
  auto res = s3d::clamp(value, lowBound, highBound);
  EXPECT_FLOAT_EQ(res, lowBound);
}

TEST_F(math_clamp_float, low_gives_low_bound) {
  auto value = lowBound;
  auto res = s3d::clamp(value, lowBound, highBound);
  EXPECT_FLOAT_EQ(res, lowBound);
}

TEST_F(math_clamp_float, inside_gives_value) {
  auto value = lowBound + 1;
  auto res = s3d::clamp(value, lowBound, highBound);
  EXPECT_FLOAT_EQ(res, value);
}

TEST_F(math_clamp_float, high_gives_high_bound) {
  auto value = highBound;
  auto res = s3d::clamp(value, lowBound, highBound);
  EXPECT_FLOAT_EQ(res, highBound);
}

TEST_F(math_clamp_float, larger_gives_high_bound) {
  auto value = highBound + 1;
  auto res = s3d::clamp(value, lowBound, highBound);
  EXPECT_FLOAT_EQ(res, highBound);
}

TEST(math_moving_average, default_smooth_is_one) {
  s3d::MovingAverage<double> a{};
  double val{25.0};
  a.addToAverage(val);
  EXPECT_DOUBLE_EQ(static_cast<double>(a), val);
}

TEST(math_moving_average, smooth_is_correct) {
  s3d::MovingAverage<double> a{1.0f, 2.0f};
  double val{25.0};
  a.addToAverage(val);
  EXPECT_DOUBLE_EQ(static_cast<double>(a), 13.0f);
}

TEST(math_moving_average, works_with_integers) {
  s3d::MovingAverage<int> a{1, 2};
  int val{25};
  a.addToAverage(val);
  EXPECT_EQ(static_cast<int>(a), 13);
}
