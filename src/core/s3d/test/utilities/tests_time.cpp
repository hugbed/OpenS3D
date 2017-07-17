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
  EXPECT_GT(std::chrono::duration_cast<std::chrono::nanoseconds>(dt).count(), 0);
}

TEST(seconds_to_ms, good_number_for_thirty) {
  auto res = s3d::seconds_to_ms(0.03f);
  EXPECT_EQ(res.count(), 30);
}

TEST(seconds_to_ms, negative_for_negative) {
  auto res = s3d::seconds_to_ms(-0.001f);
  EXPECT_EQ(res.count(), -1);
}
