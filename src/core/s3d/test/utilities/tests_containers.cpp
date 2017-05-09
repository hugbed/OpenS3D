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
