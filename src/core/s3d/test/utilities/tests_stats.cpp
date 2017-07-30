#include "gtest/gtest.h"

#include "s3d/utilities/stats.h"

#include <numeric>

TEST(percentile, first_last_elements) {
  std::vector<float> v;
  v.resize(100);
  std::iota(std::begin(v), std::end(v), 0.0f);
  EXPECT_EQ(s3d::percentile(v, 0.00), 0.0f);
  EXPECT_EQ(s3d::percentile(v, 1.00), 99.0f);
}

TEST(percentile, median_tests) {
  // median
  std::vector<float> f = {15.0f, 20.0f, 35.0f, 40.0f, 50.0f};
  EXPECT_FLOAT_EQ(s3d::percentile(f, 0.5), 35.0f);

  // median shuffled
  f = {7, 10, 1, 2, 3, 4};
  EXPECT_FLOAT_EQ(s3d::percentile(f, 0.5), 3.5f);
}

TEST(percentile, other_tests) {
  std::vector<float> f = {15, 20, 35, 40, 50};
  EXPECT_FLOAT_EQ(s3d::percentile(f, 0.40), 29.0f);

  f = {1, 2, 3, 4};
  EXPECT_FLOAT_EQ(s3d::percentile(f, 0.75), 3.25f);
}
