#include "s3d/multiview/stan_fundamental_matrix_solver.h"

#include "s3d/utilities/eigen.h"

namespace s3d {

using SampleType = StanFundamentalMatrixSolver::SampleType;
using ModelType = StanFundamentalMatrixSolver::ModelType;

// static
ModelType StanFundamentalMatrixSolver::ComputeModel(const std::vector<SampleType>& pts1,
                                                    const std::vector<SampleType>& pts2) {
  assert(pts1.size() == pts2.size());

  Eigen::MatrixXd A;
  Eigen::VectorXd b;
  std::tie(A, b) = BuildEquationSystem(pts1, pts2);

  // solve
  auto x = s3d::pseudoinverse(A) * b;

  return {x[0], x[1], x[2], x[3], x[4], 0, 0};
}

// static
std::pair<Eigen::MatrixXd, Eigen::VectorXd> StanFundamentalMatrixSolver::BuildEquationSystem(
    const std::vector<SampleType>& pts1,
    const std::vector<SampleType>& pts2) {
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

  return {A, b};
}

// static
Eigen::Matrix3d StanFundamentalMatrixSolver::FundamentalMatrixFromAlignment(
    const StanAlignment& x) {
  Eigen::Matrix3d F;
  F << 0, (-x.ch_z_f + x.a_y_f), (x.ch_y + x.a_z),  //
      (x.ch_z_f), (-x.a_x_f), (-1 + x.a_f),         //
      (-x.ch_y), 1, (-x.f_a_x);
  return F;
}
}  // namespace s3d
