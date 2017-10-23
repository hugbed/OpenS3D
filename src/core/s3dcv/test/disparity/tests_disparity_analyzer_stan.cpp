#include "gtest/gtest.h"

#include "s3d/cv/disparity/disparity_analyzer_stan.h"

using s3d::DisparityAnalyzerSTAN;
using s3d::StanAlignment;

bool alignmentEqual(StanAlignment a, StanAlignment b) {
  return std::make_tuple(a.f_a_x, a.ch_y, a.a_z, a.a_y_f, a.a_x_f, a.a_f, a.ch_z_f) ==
         std::make_tuple(b.f_a_x, b.ch_y, b.a_z, b.a_y_f, b.a_x_f, b.a_f, b.ch_z_f);
}

TEST(disparity_analyzer_stan_results, results_to_alignment) {
  DisparityAnalyzerSTAN::Results results;
  StanAlignment alignment;
  alignment.ch_z_f = 1.0f;
  alignment.a_f = 2.0f;
  alignment.a_x_f = 3.0f;
  alignment.a_y_f = 4.0f;
  alignment.a_z = 5.0f;
  alignment.ch_y = 6.0f;
  alignment.f_a_x = 7.0f;

  results.updateParameters(0.0, 0.1, 1.0f, alignment);
  StanAlignment resultsAlignment = results.getStanAlignment();

  EXPECT_TRUE(alignmentEqual(alignment, resultsAlignment));
}
