#ifndef S3D_ROBUST_PARAMETERS_H
#define S3D_ROBUST_PARAMETERS_H

#include <cstddef>

namespace s3d::robust {

struct Parameters {
  size_t nbTrials{500};
  double distanceThreshold{0.01};
  double confidence{0.999};
};

} // namespace s3d::robust

#endif //S3D_ROBUST_PARAMETERS_H
