#ifndef S3D_MULTIVIEW_STAN_FUNDAMENTAL_MATRIX_SOLVER_H
#define S3D_MULTIVIEW_STAN_FUNDAMENTAL_MATRIX_SOLVER_H

#include "s3d/robust_estimation/robust_estimation_traits.h"

// std::vector requires complete type
#include <Eigen/Dense>

#include <vector>

namespace s3d {

// todo: this is really not clear
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
  using SampleType = Eigen::Vector3d;
  using ModelType = StanAlignment;

  static ModelType ComputeModel(const std::vector<SampleType>& pts1,
                                const std::vector<SampleType>& pts2);

  static std::pair<Eigen::MatrixXd, Eigen::VectorXd> BuildEquationSystem(
      const std::vector<SampleType>& pts1,
      const std::vector<SampleType>& pts2);

  static Eigen::Matrix3d FundamentalMatrixFromAlignment(const StanAlignment& x);
};

template <>
struct robust_solver_traits<StanFundamentalMatrixSolver> {
  using SampleType = StanFundamentalMatrixSolver::SampleType;
  using ModelType = StanFundamentalMatrixSolver::ModelType;
  enum { MIN_NB_SAMPLES = 5 };
};

}  // namespace s3d

#endif  // S3D_MULTIVIEW_STAN_FUNDAMENTAL_MATRIX_SOLVER_H
