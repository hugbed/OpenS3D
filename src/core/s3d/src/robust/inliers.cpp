#include "s3d/robust/inliers.h"

#include "s3d/utilities/stats.h"

#include <tuple>

namespace s3d::robust {

Inliers::Inliers(size_t nbPts, double distanceThreshold)
  : distanceThreshold_{distanceThreshold}
  , current_(nbPts)
  , best_(nbPts)
{
}

bool Inliers::currentInliersAreBetter() const {
  return currentNb_ > bestNb_;
}

void Inliers::chooseCurrentInliersAsBest() {
  bestNb_ = currentNb_;
  std::copy(std::begin(current_), std::end(current_), std::begin(best_));
}

void Inliers::updateBest() {
  lastWasBest_ = currentInliersAreBetter();
  if (lastWasBest_) {
    chooseCurrentInliersAsBest();
  }
}

void Inliers::updateCurrent(const std::vector<double> &distances) {
  std::tie(current_, currentNb_) = computeInliers(distances);
}

std::pair<std::vector<bool>, size_t> Inliers::computeInliers(const std::vector<double> &distances) {
  return Inliers::computeInliers(distances, distanceThreshold_);
}

std::pair<std::vector<bool>, size_t> Inliers::computeInliers(const std::vector<double> &distances, double distanceThreshold) {
  std::vector<bool> inliers(distances.size());
  size_t nbInliers = 0;
  for (auto i = 0ULL; i < distances.size(); ++i) {
    if (distances[i] <= distanceThreshold) {
      inliers[i] = true;
      nbInliers++;
    }
  }
  return {inliers, nbInliers};
}

size_t Inliers::getCurrentNb() const noexcept {
  return currentNb_;
}

size_t Inliers::getBestNb() const noexcept {
  return bestNb_;
}

double Inliers::getDistanceThreshold() const {
  return distanceThreshold_;
}

const std::vector<bool> &Inliers::getBest() const noexcept {
  return best_;
}

bool Inliers::wereLastBest() {
  return lastWasBest_;
}


InliersLMedS::InliersLMedS(size_t nbPts, double distanceThreshold) : Inliers(nbPts, distanceThreshold) {}

void InliersLMedS::updateCurrent(const std::vector<double> &distances) {
  distanceThreshold_ = s3d::median(distances);
}

bool InliersLMedS::currentInliersAreBetter() const {
  return distanceThreshold_ < bestDist_;
}

void InliersLMedS::chooseCurrentInliersAsBest() {
  bestDist_ = distanceThreshold_;
}

std::pair<std::vector<bool>, size_t> InliersLMedS::computeInliers(const std::vector<double> &distances) {
  return Inliers::computeInliers(distances, bestDist_);
}


} // namespace s3d::robust