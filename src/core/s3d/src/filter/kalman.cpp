#include "s3d/filter/kalman.h"

namespace s3d::filter {

Kalman::Kalman(int nbParameters)
        : x(Eigen::VectorXd::Zero(nbParameters)),
          P(Eigen::MatrixXd::Identity(nbParameters, nbParameters)),
          R(Eigen::MatrixXd::Zero(nbParameters, nbParameters)) // no variance
{}

void Kalman::setInitialValues(Eigen::VectorXd values) {
  const long nbParameters = std::max(values.cols(), values.rows());
  x = std::move(values);
  P = Eigen::MatrixXd::Identity(nbParameters, nbParameters);
}

void Kalman::setVariances(const Eigen::VectorXd &variances) {
  R = variances.asDiagonal();
}

void Kalman::filter(const Eigen::VectorXd &z) {
  innovationFromObservation(z);
  updateEstimation();
};

Eigen::VectorXd &Kalman::getEstimation() {
  return x;
}

void Kalman::updateEstimation() {
  x = x + innovation.K * innovation.y;
  P = P - innovation.K * P;
}

void Kalman::innovationFromObservation(const Eigen::VectorXd &z) {
  innovation.y = z - x;
  innovation.S = P + R;
  innovation.K = P * innovation.S.inverse();
}

} // namespace s3d::filter