#ifndef FILTER_STAN_VARIANCE_H
#define FILTER_STAN_VARIANCE_H

#include "s3d/utilities/eigen.h"

namespace s3d {

struct StanVariance {
  double roll{};
  double vertical{};
  double panKeystone{};
  double tiltKeystone{};
  double tiltOffset{};
  double zoom{};
  double zParallaxDeformation{};

  Eigen::VectorXd toVector() const {
    Eigen::VectorXd varianceVector(7);
    varianceVector << vertical, roll, zoom, tiltOffset, panKeystone, tiltKeystone, zParallaxDeformation;
    return varianceVector;
  }
};

}

#endif //FILTER_STAN_VARIANCE_H
