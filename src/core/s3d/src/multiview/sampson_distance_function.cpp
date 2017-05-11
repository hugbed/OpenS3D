#include "s3d/multiview/sampson_distance_function.h"

namespace s3d {

// static
void SampsonDistanceFunction::ComputeDistance(const std::vector<PointsType>& pts1,
                                              const std::vector<PointsType>& pts2,
                                              const StanFundamentalMatrixSolver::ModelType& model,
                                              std::vector<double>* distances) {
  assert(pts1.size() == pts2.size());
  assert(pts1.size() == distances->size());

  // create matrices with pts1h, pts2h
  Eigen::MatrixXd pts1h(3, pts1.size()), pts2h(3, pts2.size());

  // fill matrices
  for (int i = 0; i < pts1.size(); ++i) {
    pts1h.block<3, 1>(0, i) = pts1[i];
    pts2h.block<3, 1>(0, i) = pts2[i];
  }

  // this is probably not efficient
  auto f = StanFundamentalMatrixSolver::FundamentalMatrixFromSevenParams(model);
  auto pfp = (pts2h.transpose() * f).transpose();
  auto pfp2 = pfp.cwiseProduct(pts1h);
  auto colSum = pfp2.colwise().sum();
  auto d = colSum.array().pow(2);

  auto epl1 = f * pts1h;
  auto epl2 = f.transpose() * pts2h;
  auto coeff = epl1.row(0).array().square() + epl1.row(1).array().square() +
               epl2.row(0).array().square() + epl2.row(1).array().square();

  auto distancesVector = d.array() / coeff.array();

  Eigen::VectorXd::Map(&distances->operator[](0), distancesVector.size()) = distancesVector;
}
}
