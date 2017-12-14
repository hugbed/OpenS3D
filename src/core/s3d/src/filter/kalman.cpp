#include "s3d/filter/kalman.h"

namespace s3d {
namespace filter {

Kalman::Kalman(int nbParameters)
        : x(Eigen::VectorXd::Zero(nbParameters)),
          P(Eigen::MatrixXd::Identity(nbParameters, nbParameters)),
          R(Eigen::MatrixXd::Zero(nbParameters, nbParameters)), // no variance
          Q(Eigen::MatrixXd::Zero(nbParameters, nbParameters))
{}

void Kalman::setInitialValues(Eigen::VectorXd values) {
  const long nbParameters = std::max(values.cols(), values.rows());
  x = std::move(values);
  P = Eigen::MatrixXd::Identity(nbParameters, nbParameters);
}

void Kalman::setObservationVariances(const Eigen::VectorXd &variances) {
  R = variances.asDiagonal();
}

void Kalman::setProcessVariances(const Eigen::VectorXd &variances) {
  Q = variances.asDiagonal();
}

void Kalman::filter(const Eigen::VectorXd &z) {
  prediction();
  innovationFromObservation(z);
  updateEstimation();
};

Eigen::VectorXd &Kalman::getEstimation() {
  return x;
}

void Kalman::prediction() {
  P = P + Q;
}

void Kalman::innovationFromObservation(const Eigen::VectorXd &z) {
  innovation.y = z - x;
  innovation.S = P + R;
  innovation.K = P * innovation.S.inverse();
}

void Kalman::updateEstimation() {
  x = x + innovation.K * innovation.y;
  P = P - innovation.K * P;
}

} // namespace filter
} // namespace s3d