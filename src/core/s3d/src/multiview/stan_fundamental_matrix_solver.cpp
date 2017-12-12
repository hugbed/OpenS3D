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
  Eigen::VectorXd x = A.colPivHouseholderQr().solve(b); // QR

  return {x[0], x[1], x[2], x[3], x[4], 0.0f, 0.0f};
}

// static
std::pair<Eigen::MatrixXd, Eigen::VectorXd> StanFundamentalMatrixSolver::BuildEquationSystem(
    const std::vector<SampleType>& pts1,
    const std::vector<SampleType>& pts2) {
  constexpr int nbVariables = robust::estimation_algorithm_traits<StanFundamentalMatrixSolver>::MIN_NB_SAMPLES;
  assert(pts1.size() >= nbVariables);
  assert(pts2.size() >= nbVariables);

  // rows x cols = nbPts x nbVariables
  Eigen::MatrixXd A(pts1.size(), nbVariables);
  Eigen::VectorXd b(pts1.size());

  for (size_t i = 0; i < pts1.size(); ++i) {
    auto x = pts1[i];
    auto xp = pts2[i];

    // fill A
    Eigen::VectorXd row(nbVariables);
    row << xp.x() - x.x(), xp.x(), xp.y(), -1, xp.x() * x.y();//, -x.y() * xp.y(),
                                                              //   x.x() * xp.y() - xp.x() * x.y();
    A.block<1, nbVariables>(i, 0) = row;

    // fill b
    b[i] = xp.y() - x.y();
  }

  return {A, b};
}

// static
Eigen::Matrix3d StanFundamentalMatrixSolver::FundamentalMatrixFromAlignment(
    const StanAlignment& a) {
  Eigen::Matrix3d F;
  F << 0, (-a.ch_z_f + a.a_y_f), (a.ch_y + a.a_z),  //
      (a.ch_z_f), (-a.a_x_f), (-1 + a.a_f),         //
      (-a.ch_y), 1, (-a.f_a_x);
  return F;
}


// static
Eigen::Matrix3d StanFundamentalMatrixSolver::CenteredFundamentalMatrixFromAlignment(
        const StanAlignment& alignment, const Size& imageSize) {
  Eigen::Matrix3d T, TInv;
  T << 1.0f, 0.0f, -static_cast<float>(imageSize.getWidth() / 2), //
       0.0f, 1.0f, -static_cast<float>(imageSize.getHeight() / 2), //
       0.0f, 0.0f, 1.0f;

  Eigen::Matrix3d F = FundamentalMatrixFromAlignment(alignment);

  return T.transpose() * F * T;
}

}  // namespace s3d
