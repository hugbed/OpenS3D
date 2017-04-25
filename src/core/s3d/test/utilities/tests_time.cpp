#include "gtest/gtest.h"

#include "s3d/utilities/time.h"

// more to test that it works than to test real time mesured
TEST(mesure_time, param_in_func_positive_time_for_loops) {
  auto dt = s3d::mesure_time(
      [](size_t N) {
        for (auto i = 0; i < N; ++i) {
          volatile int value;
          value = 2;
        }
      },
      100);
  EXPECT_GT(std::chrono::duration_cast<std::chrono::nanoseconds>(dt).count(), 0);
}

// this seems faster, maybe...
TEST(mesure_time, param_capture_lambda_positive_time_for_loops) {
  size_t N = 100;
  auto dt = s3d::mesure_time([&]() {
    for (auto i = 0; i < N; ++i) {
      volatile int value;
      value = 2;
    }
  });
  EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(dt).count(), 0);
}
