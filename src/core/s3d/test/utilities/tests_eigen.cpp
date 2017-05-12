#include "gtest/gtest.h"

#include "s3d/utilities/eigen.h"

using s3d::pseudoinverse;

TEST(eigen, pseudo_inverse_simple_case_dynamic_matrix) {
  Eigen::MatrixXd m(2, 2);
  m << 1.0, 2.0,  //
      3.0, 4.0;
  auto res = pseudoinverse(m);
  EXPECT_DOUBLE_EQ(res(0, 0), -2.0);
  EXPECT_DOUBLE_EQ(res(0, 1), 1.0);
  EXPECT_DOUBLE_EQ(res(1, 0), 1.5);
  EXPECT_DOUBLE_EQ(res(1, 1), -0.5);
}

// for square fixed size, call matrix.inverse() instead
TEST(eigen, pseudo_inverse_square_matrix_is_inverse) {
  Eigen::Matrix2d mFixed;
  mFixed << 1.0, 2.0,  //
      3.0, 4.0;
  Eigen::MatrixXd mDynamic(mFixed);

  auto pinvRes = pseudoinverse(mDynamic);
  auto invRes = mFixed.inverse();

  for (int i = 0; i < invRes.size(); ++i) {
    EXPECT_DOUBLE_EQ(pinvRes(i), invRes(i));
  }
}
