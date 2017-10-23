#include "gtest/gtest.h"

#include "s3d/rectification/rectification_stan.h"

using s3d::RectificationStan;

// vertical offset offsets in pixels
// roll angle rotates only
// zoom unzooms by X % (scale is coherent)
// tilt offset ?
// panKeystone: regression testing
// tiltKeystone: regression testing
// zParallax deformation: regression testing. (though it may be hardcoded to 0 in
// rectification_stan)

TEST(rectification_stan, left_matrix_regression) {
  s3d::StanAlignment a;
  constexpr auto PI = 3.14159265358979323846f;
  a.ch_y = PI / 128;
  a.a_z = PI / 64;
  a.a_f = 0.1;
  a.f_a_x = 40;
  a.a_y_f = 0.05;
  a.a_x_f = 0.04;
  a.ch_z_f = 0.03;

  Eigen::Matrix3f H = RectificationStan::leftImageMatrix(a);

  EXPECT_FLOAT_EQ(H(0, 0), 1.0);
  EXPECT_FLOAT_EQ(H(0, 1), 0.024543693);
  EXPECT_FLOAT_EQ(H(0, 2), 0.0);
  EXPECT_FLOAT_EQ(H(1, 0), -0.024543693f);
  EXPECT_FLOAT_EQ(H(1, 1), 1.0);
  EXPECT_FLOAT_EQ(H(1, 2), 0.0f);
  EXPECT_FLOAT_EQ(H(2, 0), -0.029999999);
  EXPECT_FLOAT_EQ(H(2, 1), 0.0);
  EXPECT_FLOAT_EQ(H(2, 2), 1);
}

TEST(rectification_stan, right_matrix_regression) {
  s3d::StanAlignment a;
  constexpr auto PI = 3.14159265358979323846f;
  a.ch_y = PI / 128;
  a.a_z = PI / 64;
  a.a_f = 0.1;
  a.f_a_x = 40;
  a.a_y_f = 0.05;
  a.a_x_f = 0.04;
  a.ch_z_f = 0.03;

  Eigen::Matrix3f H = RectificationStan::rightImageMatrix(a);

  EXPECT_FLOAT_EQ(H(0, 0), 0.89999998);
  EXPECT_FLOAT_EQ(H(0, 1), 0.073631078);
  EXPECT_FLOAT_EQ(H(0, 2), 0.0);
  EXPECT_FLOAT_EQ(H(1, 0), -0.073631078);
  EXPECT_FLOAT_EQ(H(1, 1), 0.89999998);
  EXPECT_FLOAT_EQ(H(1, 2), 40);
  EXPECT_FLOAT_EQ(H(2, 0), 0.02);
  EXPECT_FLOAT_EQ(H(2, 1), -0.039999999);
  EXPECT_FLOAT_EQ(H(2, 2), 1);
}