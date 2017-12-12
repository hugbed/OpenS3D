//
// Created by jon on 12/12/17.
//

#ifndef S3D_FILTER_KALMAN_FILTER_H
#define S3D_FILTER_KALMAN_FILTER_H

#include "Eigen/Dense"

namespace s3d {
namespace filter {

class Kalman {
public:
  explicit Kalman(int nbParameters);

  void setInitialValues(Eigen::VectorXd values);

  void setVariances(const Eigen::VectorXd& variances);

  void filter(const Eigen::VectorXd& z);

  Eigen::VectorXd& getEstimation();

private:
  void updateEstimation();

  void innovationFromObservation(const Eigen::VectorXd& z);

  struct Innovation {
    Eigen::VectorXd y;
    Eigen::MatrixXd S;
    Eigen::MatrixXd K;
  };

  Innovation innovation;
  Eigen::VectorXd x;
  Eigen::MatrixXd P;
  Eigen::MatrixXd R;
};

} // namespace filter
} // namespace s3d

#endif // S3D_FILTER_KALMAN_FILTER_H
