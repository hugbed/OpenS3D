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

TEST(median, one_value) {
  std::vector<double> values = { 1.0 };
  EXPECT_DOUBLE_EQ(values[0], s3d::median(values));
}

TEST(median, pair_value) {
  std::vector<double> values = { 1.0, 2.0 };
  EXPECT_DOUBLE_EQ((values[0] + values[1]) / 2.0, s3d::median(values));
}

TEST(median, odd_value) {
  std::vector<double> values = { 1.0, 2.0, 3.0 };
  EXPECT_DOUBLE_EQ(values[1], s3d::median(values));
}

double mean(const std::vector<double>& values) {
  double sum{0.0};
  for (auto value : values) {
    sum += value;
  }
  return sum / static_cast<double>(values.size());
}

double offlineVariance(const std::vector<double>& values) {
  double meanValues = mean(values);
  double sumDistSquared{0.0};
  for (auto value : values) {
    sumDistSquared += (value - meanValues) * (value - meanValues);
  }
  return sumDistSquared / (static_cast<double>(values.size()) - 1.0);
}

TEST(online_variance, compare_results) {
  std::vector<double> values = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9};
  double goldVariance = offlineVariance(values);

  s3d::OnlineVariance<double> onlineVariance;

  for (auto value : values) {
    onlineVariance.update(value);
  }

  EXPECT_DOUBLE_EQ(onlineVariance.getVariance(), goldVariance);
}