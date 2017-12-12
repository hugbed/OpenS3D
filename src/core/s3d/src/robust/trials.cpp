#include "s3d/robust/trials.h"

#include <limits>

#include <cmath>

namespace s3d::robust {

Trials::Trials(size_t nbPts, size_t minNbSamples, size_t maxNbTrials, Parameters params)
        : params_(params),
          maxNb_{maxNbTrials},
          minNbSamples_{minNbSamples},
          logOneMinusConf_{log(1.0 - params.confidence)},
          oneOverNbPts_{1.0 / nbPts} {}

void Trials::updateNb(double currentNbInliers) {
  constexpr double eps = 1E-15;

  size_t newNb = 0;
  double ratioOfInliers = currentNbInliers * oneOverNbPts_;
  if (ratioOfInliers <= (1 - eps)) {
    auto ratioPow = std::pow(ratioOfInliers, minNbSamples_);
    if (ratioPow > eps) {
      auto logOneMinusRatioPow = std::log(1 - ratioPow);
      newNb = size_t(std::ceil(logOneMinusConf_ / logOneMinusRatioPow));
    } else {
      newNb = std::numeric_limits<size_t>::max();
    }
  }

  if (maxNb_ > newNb) {
    maxNb_ = newNb;
  }
}

bool Trials::reachedMaxNb() {
  bool hasReached = curNb_ >= maxNb_;
  curNb_++;
  return hasReached;
}

size_t Trials::currentNb() {
  return curNb_;
}

} // namespace s3d::robust