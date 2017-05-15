#include "gtest/gtest.h"

#include "s3d/utilities/containers.h"

TEST(values_from_indices, normal_use) {
  std::vector<int> values = {11, 22, 33, 44};
  std::vector<int> indices = {1, 3};
  auto res = s3d::values_from_indices(values, indices);
  EXPECT_EQ(res.size(), 2);
  EXPECT_EQ(res[0], values[indices[0]]);
  EXPECT_EQ(res[1], values[indices[1]]);
}

TEST(values_from_indices, first_last_index) {
  std::vector<int> values = {11, 22, 33, 44};
  std::vector<int> indices = {0, static_cast<int>(values.size() - 1)};
  auto res = s3d::values_from_indices(values, indices);
  EXPECT_EQ(res.size(), 2);
  EXPECT_EQ(res[0], values[indices[0]]);
  EXPECT_EQ(res[1], values[indices[indices.size() - 1]]);
}

TEST(center, res_is_values_minus_center) {
  constexpr int center = 2;
  std::vector<int> values{-2, 0, 2, 4};
  std::vector<int> gold;
  for (auto value: values) {
    gold.emplace_back(value - center);
  }

  s3d::center_values(std::begin(values), std::end(values), std::begin(values), center);

  for (int i = 0; i < values.size(); ++i) {
    EXPECT_EQ(values[i], gold[i]);
  }
}
