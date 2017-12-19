#ifndef FILTER_STAN_VARIANCE_H
#define FILTER_STAN_VARIANCE_H

#include <Eigen/Dense>

namespace s3d {

struct StanVariance {
  double roll{};
  double vertical{};
  double panKeystone{};
  double tiltKeystone{};
  double tiltOffset{};
  double zoom{};

  Eigen::VectorXd toVector() const {
    Eigen::VectorXd varianceVector(5);
    varianceVector << vertical, roll, zoom, tiltOffset, panKeystone;
    return varianceVector;
  }
};

}

#endif //FILTER_STAN_VARIANCE_H
