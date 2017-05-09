#include "gtest/gtest.h"

#include "s3d/utilities/rand.h"

TEST(rand_n_values, values_are_correct_seed_0) {
  constexpr auto seed = 0;
  constexpr auto minVal = 0;
  constexpr auto maxVal = 10;
  constexpr auto nbValues = 4;

  auto res = s3d::rand_n_unique_values(minVal, maxVal, nbValues, seed);
  EXPECT_EQ(res[0], 6);
  EXPECT_EQ(res[1], 7);
  EXPECT_EQ(res[2], 9);
  EXPECT_EQ(res[3], 5);
}
