//
// Created by jon on 12/11/17.
//

#ifndef S3D_ROBUST_TRIALS_H
#define S3D_ROBUST_TRIALS_H

#include "parameters.h"

#include <cstddef>

namespace s3d::robust {

class Trials {
public:
  explicit Trials(size_t nbPts, size_t minNbSamples, size_t maxNbTrials, Parameters params);

  void updateNb(double currentNbInliers);

  bool reachedMaxNb();

  size_t currentNb();

private:
  Parameters params_;
  size_t maxNb_{};
  size_t minNbSamples_{};
  size_t curNb_{0};

  const double logOneMinusConf_;
  const double oneOverNbPts_;
};

} // namespace s3d::robust

#endif //S3D_ROBUST_TRIALS_H
