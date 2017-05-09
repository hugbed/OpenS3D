#include "gtest/gtest.h"

#include "s3d/robust_estimation/ransac.h"

TEST(values_from_indices, normal_use) {
  std::vector<int> values = {11, 22, 33, 44};
  std::vector<int> indices = {1, 3};
  auto res = s3d::values_from_indices(values, indices);
  EXPECT_EQ(res.size(), 2);
  EXPECT_EQ(res[0], values[indices[0]]);
  EXPECT_EQ(res[1], values[indices[1]]);
}

TEST(values_from_indices, first_last_index) {
  std::vector<int> values = {11, 22, 33, 44};
  std::vector<int> indices = {0, static_cast<int>(values.size() - 1)};
  auto res = s3d::values_from_indices(values, indices);
  EXPECT_EQ(res.size(), 2);
  EXPECT_EQ(res[0], values[indices[0]]);
  EXPECT_EQ(res[1], values[indices[indices.size() - 1]]);
}

TEST(rand_n_values, values_are_correct_seed_0) {
  constexpr auto seed = 0;
  constexpr auto minVal = 0;
  constexpr auto maxVal = 10;
  constexpr auto nbValues = 4;

  auto res = s3d::rand_n_unique_values(minVal, maxVal, nbValues, seed);
  EXPECT_EQ(res[0], 6);
  EXPECT_EQ(res[1], 7);
  EXPECT_EQ(res[2], 9);
  EXPECT_EQ(res[3], 5);
}

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
    auto dNorm = sqrt(dx*dx + dy*dy);

    auto A = -dy / dNorm;
    auto B = dx / dNorm;
    auto C = A*x[0] + B*y[0];
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
  using Ransac = Ransac<LineSolver, LeastSquareDistanceFunction>;
  using PointsType = LineSolver::PointsType;

  Ransac::Params params{};
  params.minNbPts = 2;
  Ransac ransac(params);

  // inliers: y = x
  auto x = std::vector<PointsType>{0, 1, 2, 3, 4, 5, 6, 7, 8};
  auto y = std::vector<PointsType>{0, 1, 2, 3, 4, 5, 6, 7, 8};

  // outliers
  x.insert(std::end(x), {1, 3, 4, 6, 8});
  y.insert(std::end(y), {5, 6, 1, 1, 3});

  auto line = ransac(x, y);

  EXPECT_DOUBLE_EQ(line.A, -line.B);
  EXPECT_DOUBLE_EQ(line.C, 0);
}
