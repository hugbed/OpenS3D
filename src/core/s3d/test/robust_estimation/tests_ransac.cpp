#include "gtest/gtest.h"

#include "s3d/robust_estimation/ransac.h"
#include "s3d/multiview/stan_fundamental_matrix_solver.h"

// Ax + By + C = 0
struct Line {
  double A;
  double B;
  double C;
};

// Example for line solver
class LineSolver {
 public:
  using PointsType = double;
  using ModelType = Line;

  static ModelType ComputeModel(const std::vector<PointsType>& x,
                                const std::vector<PointsType>& y) {
    assert(x.size() >= 2);
    assert(y.size() >= 2);

    // distance between pts
    auto dx = x[1] - x[0];
    auto dy = y[1] - y[0];
    auto dNorm = sqrt(dx * dx + dy * dy);

    auto A = -dy / dNorm;
    auto B = dx / dNorm;
    auto C = A * x[0] + B * y[0];
    return {A, B, C};
  }
};

// Distance that can be applied to the model
class LeastSquareDistanceFunction {
 public:
  using PointsType = LineSolver::PointsType;

  static void ComputeDistance(const std::vector<PointsType>& x,
                              const std::vector<PointsType>& y,
                              const LineSolver::ModelType& model,
                              std::vector<double>* distances) {
    assert(x.size() == y.size());
    assert(x.size() == distances->size());
    for (size_t i = 0; i < distances->size(); i++) {
      distances->operator[](i) = model.A * x[i] + model.B * y[i] + model.C;
    }
  }
};

TEST(ransac, line_solver) {
  Ransac::Params params{};
  params.minNbPts = 2;
  RansacAlgorithm<LineSolver, LeastSquareDistanceFunction> ransac(params);

  // inliers: y = x
  using PointsType = LineSolver::PointsType;
  auto x = std::vector<PointsType>{0, 1, 2, 3, 4, 5, 6, 7, 8};
  auto y = std::vector<PointsType>{0, 1, 2, 3, 4, 5, 6, 7, 8};

  // outliers
  x.insert(std::end(x), {1, 3, 4, 6, 8});
  y.insert(std::end(y), {5, 6, 1, 1, 3});

  auto line = ransac(x, y);

  EXPECT_DOUBLE_EQ(line.A, -line.B);
  EXPECT_DOUBLE_EQ(line.C, 0);
}


class FakeDistanceAllOverThreshold {
 public:
  using PointsType = LineSolver::PointsType;

  static constexpr auto THRESHOLD = 0.0;

  static void ComputeDistance(const std::vector<PointsType>& x,
                              const std::vector<PointsType>& y,
                              const LineSolver::ModelType& model,
                              std::vector<double>* distances) {
    for (auto i = 0ULL; i < distances->size(); ++i) {
      distances->operator[](i) = THRESHOLD + 1;
    }
  }
};

TEST(ransac, not_enough_inliers_throws) {
  Ransac::Params params{};
  params.minNbPts = 2;
  params.distanceThreshold = FakeDistanceAllOverThreshold::THRESHOLD;
  RansacAlgorithm<LineSolver, LeastSquareDistanceFunction> ransac(params);

  EXPECT_THROW(ransac({0, 0}, {0, 0}), NotEnoughInliersFound);
}
