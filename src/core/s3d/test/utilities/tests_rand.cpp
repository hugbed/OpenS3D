#include "gtest/gtest.h"

#include "s3d/utilities/rand.h"

// looks like mt gives different results on different platforms
// so maybe this test is not optimal
std::vector<int> getExpectedValues() {
#ifdef _WIN32
  return {10, 7, 1, 3};
#else
  return {6, 7, 9, 5};
#endif
}

TEST(rand_n_values, values_are_correct_seed_0) {
  constexpr auto seed = 0;
  constexpr auto minVal = 0;
  constexpr auto maxVal = 10;
  constexpr auto nbValues = 4;

  auto expectedValues = getExpectedValues();

  auto res = s3d::rand_n_unique_values(minVal, maxVal, nbValues, seed);
  EXPECT_EQ(res[0], expectedValues[0]);
  EXPECT_EQ(res[1], expectedValues[1]);
  EXPECT_EQ(res[2], expectedValues[2]);
  EXPECT_EQ(res[3], expectedValues[3]);
}
