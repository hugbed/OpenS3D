#ifndef S3D_MULTIVIEW_STAN_FUNDAMENTAL_MATRIX_SOLVER_H
#define S3D_MULTIVIEW_STAN_FUNDAMENTAL_MATRIX_SOLVER_H

// std::vector requires complete type
#include <Eigen/Dense>

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
  using PointsType = Eigen::Vector3d;
  using ModelType = StanAlignment;

  static ModelType ComputeModel(const std::vector<PointsType>& pts1,
                                const std::vector<PointsType>& pts2);

  static std::pair<Eigen::MatrixXd, Eigen::VectorXd> BuildEquationSystem(
      const std::vector<PointsType>& pts1,
      const std::vector<PointsType>& pts2);

  static Eigen::Matrix3d FundamentalMatrixFromAlignment(const StanAlignment& x);
};

}  // namespace s3d

#endif  // S3D_MULTIVIEW_STAN_FUNDAMENTAL_MATRIX_SOLVER_H
