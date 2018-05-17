#ifndef S3D_MULTIVIEW_STAN_FUNDAMENTAL_MATRIX_SOLVER_H
#define S3D_MULTIVIEW_STAN_FUNDAMENTAL_MATRIX_SOLVER_H

#include "s3d/robust/estimation_algorithm_traits.h"
#include "s3d/multiview/stan_alignment.h"
#include "s3d/geometry/size.h"

// std::vector requires complete type
#include "s3d/utilities/eigen.h"

#include <vector>

namespace s3d {

class StanFundamentalMatrixSolver {
 public:
  using SampleType = Eigen::Vector3d;
  using ModelType = StanAlignment;

  static ModelType ComputeModel(const std::vector<SampleType>& pts1,
                                const std::vector<SampleType>& pts2);

  static std::pair<Eigen::MatrixXd, Eigen::VectorXd> BuildEquationSystem(
      const std::vector<SampleType>& pts1,
      const std::vector<SampleType>& pts2);

  static Eigen::Matrix3d FundamentalMatrixFromAlignment(const StanAlignment& a);

  static Eigen::Matrix3d CenteredFundamentalMatrixFromAlignment(const StanAlignment& a, const Size& imageSize);
};

namespace robust {

template<>
struct estimation_algorithm_traits<StanFundamentalMatrixSolver> {
  using SampleType = StanFundamentalMatrixSolver::SampleType;
  using ModelType = StanFundamentalMatrixSolver::ModelType;
  enum { MIN_NB_SAMPLES = 7 };
};

} // namespace robust

}  // namespace s3d

#endif  // S3D_MULTIVIEW_STAN_FUNDAMENTAL_MATRIX_SOLVER_H
