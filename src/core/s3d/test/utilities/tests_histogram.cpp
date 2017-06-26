#include "gtest/gtest.h"

#include "s3d/utilities/histogram.h"

TEST(histogram, min) {
  std::vector<double> v;
  v.resize(10);
  std::iota(std::begin(v), std::end(v), 0.0);
  EXPECT_DOUBLE_EQ(Histogram::Min(v), 0.0);
}

TEST(histogram, max) {
  std::vector<double> v;
  v.resize(10);
  std::iota(std::begin(v), std::end(v), 0.0);
  EXPECT_DOUBLE_EQ(Histogram::Max(v), 9.0);
}

TEST(histogram, bin_size_trivial) {
  Histogram hist;
  constexpr double min = 0.0;
  constexpr double max = 10.0;
  constexpr double nbBins = 10.0;
  EXPECT_DOUBLE_EQ(Histogram::BinSize(min, max, nbBins), 1.0);
}

TEST(histogram, bin_size_less_trivial) {
  Histogram hist;
  constexpr double min = -15.0;
  constexpr double max = 5.5;
  constexpr double nbBins = 5.0;
  EXPECT_NEAR(Histogram::BinSize(min, max, nbBins), 4.09, 0.01);
}

TEST(histogram, bin_index) {
  auto value = std::vector<double>{1.5, 2.5, 2.75, 11.4};
  auto min = std::vector<double>{0.0, 1.0, 1.5, 11.2};
  auto binSize = std::vector<double>{1.0, 0.5, 1.0, 0.2};
  auto expected = std::vector<int>{1, 3, 1, 1};

  for (int i = 0; i < value.size(); ++i) {
    EXPECT_DOUBLE_EQ(Histogram::BinIdx(value[i], min[i], binSize[i]), expected[i]);
  }
}

TEST(histogram, bin_index_max_value_no_overflow) {
  auto values = std::vector<double>{0.0, 1.0, 2.0, 3.0, 4.0, 5.0};
  auto hist = Histogram::Compute(values, 5);
  EXPECT_GT(hist.binSize, 1.0 + 0.000000001);
}

TEST(histogram, compute) {
  auto values = std::vector<double>{1.5, 2.5, 4.6, 7.3, 8.9, 11.4};
  auto hist = Histogram::Compute(values, 3);
  EXPECT_EQ(hist.counts.size(), 3);
  EXPECT_EQ(hist.counts[0], 3);
  EXPECT_EQ(hist.counts[1], 1);
  EXPECT_EQ(hist.counts[2], 2);
}
