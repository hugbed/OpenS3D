#include "s3d/robust_estimation/ransac.h"

namespace s3d {

// Inliers
Ransac::Inliers::Inliers(size_t nbPts) {
  current_.resize(nbPts);
  best_.resize(nbPts);
}

bool Ransac::Inliers::currentInliersAreBetter() const noexcept {
  return currentNb_ > bestNb_;
}

void Ransac::Inliers::chooseCurrentInliersAsBest() {
  bestNb_ = currentNb_;
  std::copy(std::begin(current_), std::end(current_), std::begin(best_));
}

void Ransac::Inliers::updateBest() {
  if (currentInliersAreBetter()) {
    chooseCurrentInliersAsBest();
  }
}

void Ransac::Inliers::updateCurrent(const Distances& distances, double distanceThreshold) {
  current_.clear();
  current_.resize(distances.size());
  currentNb_ = 0;
  for (auto i = 0ULL; i < distances.size(); ++i) {
    if (distances[i] < distanceThreshold) {
      current_[i] = true;
      currentNb_++;
    }
  }
}

size_t Ransac::Inliers::getCurrentNb() const noexcept {
  return currentNb_;
}

size_t Ransac::Inliers::getBestNb() const noexcept {
  return bestNb_;
}

const std::vector<bool>& Ransac::Inliers::getBest() const noexcept {
  return best_;
}

// Trials
Ransac::Trials::Trials(size_t nbPts, size_t minNbSamples, size_t maxNbTrials, Params params)
    : params_(params),
      maxNb_{maxNbTrials},
      minNbSamples_{minNbSamples},
      logOneMinusConf_{log(1.0 - params.confidence)},
      oneOverNbPts_{1.0 / nbPts} {}

void Ransac::Trials::updateNb(double currentNbInliers) {
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

bool Ransac::Trials::reachedMaxNb() {
  bool hasReached = curNb_ >= maxNb_;
  curNb_++;
  return hasReached;
}

size_t Ransac::Trials::currentNb() {
  return curNb_;
}
}  // namespace s3d
