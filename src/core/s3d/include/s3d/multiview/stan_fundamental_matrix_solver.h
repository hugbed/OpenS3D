#ifndef S3D_MULTIVIEW_STAN_FUNDAMENTAL_MATRIX_SOLVER_H
#define S3D_MULTIVIEW_STAN_FUNDAMENTAL_MATRIX_SOLVER_H

#include <iostream>
#include "s3d/utilities/eigen.h"

struct StanAlignment {
  double ch_y{};
  double a_z{};
  double a_f{};
  double f_a_x{};
  double a_y_f{};
  double a_x_f{};
  double ch_z_f{};
};

class StanFundamentalMatrixSolver {
 public:
  using PointsType = Eigen::Vector3d;
  using ModelType = StanAlignment;

  static ModelType ComputeModel(const std::vector<PointsType>& pts1,
                                const std::vector<PointsType>& pts2) {
    assert(pts1.size() == pts2.size());

    constexpr int nbVariables = 5;
    assert(pts1.size() >= nbVariables);
    assert(pts2.size() >= nbVariables);

    // rows x cols = nbPts x 6
    Eigen::MatrixXd A(pts1.size(), nbVariables + 1);
    Eigen::VectorXd b(pts1.size());

    for (int i = 0; i < pts1.size(); ++i) {
      auto x = pts1[i];
      auto xp = pts2[i];

      // fill A
      Eigen::VectorXd row(nbVariables + 1);
      row << xp.x() - x.x(), xp.x(), xp.y(), -1, xp.x() * x.y(), -x.y() * xp.y();
      A.block<1, nbVariables + 1>(i, 0) = row;

      // fill b
      b[i] = xp.y() - x.y();
    }

    // solve
    auto x = s3d::pinv(A) * b;

    return {x[0], x[1], x[2], x[3], x[4], 0, 0};
  }

  static Eigen::Matrix3d FundamentalMatrixFromSevenParams(const StanAlignment x) {
    Eigen::Matrix3d F;
    F << 0, (-x.ch_z_f + x.a_y_f), (x.ch_y + x.a_z), x.ch_z_f, -x.a_x_f, (-1 + x.a_f), -x.ch_y, 1,
        -x.f_a_x;
    return F;
  }
};

#endif  // S3D_MULTIVIEW_STAN_FUNDAMENTAL_MATRIX_SOLVER_H
