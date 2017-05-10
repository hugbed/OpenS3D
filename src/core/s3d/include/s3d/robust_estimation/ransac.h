#ifndef S3D_ROBUST_ESTIMATION_RANSAC_H
#define S3D_ROBUST_ESTIMATION_RANSAC_H

#include "s3d/utilities/rand.h"
#include "s3d/utilities/containers.h"

#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <memory>
#include <iostream>

// todo: put this in some more restricted scope (namespace robust?)
class NotEnoughInliersFound : std::runtime_error {
 public:
  NotEnoughInliersFound(size_t nbInliers, size_t expectedNb)
      : std::runtime_error(std::string("Not enough inliers found: [expected, found] = [") +
                           std::to_string(nbInliers) +
                           std::string(", ") +
                           std::to_string(expectedNb) +
                           std::string("]")) {}
};

// Empty base class for common types
class Ransac {
 public:
  using Distances = std::vector<double>;

  struct Params {
    size_t nbTrials{500};
    double distanceThreshold{0.01};
    double confidence{0.999};
    size_t minNbPts{};
  };

 protected:
  // Base class only, instantiate RansacAlgorithm instead
  Ransac() = default;

  class Inliers {
   public:
    explicit Inliers(size_t nbPts);

    bool currentInliersAreBetter() const noexcept;
    void chooseCurrentInliersAsBest();
    void updateBest();
    void updateCurrent(const Distances& distances, double distanceThreshold);

    size_t getCurrentNb() const noexcept;
    size_t getBestNb() const noexcept;
    const std::vector<bool>& getBest() const noexcept;

   private:
    size_t currentNb_{0};
    std::vector<bool> current_;
    size_t bestNb_{0};
    std::vector<bool> best_;
  };

  class Trials {
   public:
    explicit Trials(size_t nbPts, size_t maxNbTrials, Params params);
    void updateNb(double currentNbInliers);
    bool reachedMaxNb();

   private:
    Params params_;
    size_t maxNb_{};
    size_t curNb_{0};

    const double logOneMinusConf_;
    const double oneOverNbPts_;
  };
};

template <class ModelSolver, class DistanceFunction>
class RansacAlgorithm : public Ransac {
 public:
  using Points = std::vector<typename ModelSolver::PointsType>;

  explicit RansacAlgorithm(Params params) : params_(params) {}

  typename ModelSolver::ModelType operator()(Points pts1, Points pts2) {
    assert(pts1.size() == pts2.size());
    assert(pts1.size() >= params_.minNbPts);

    pts1_ = std::move(pts1);
    pts2_ = std::move(pts2);
    trials_ = std::make_unique<Trials>(pts1_.size(), params_.nbTrials, params_);
    inliers_ = std::make_unique<Inliers>(pts1_.size());
    distances_.resize(pts1_.size());

    return runAlgorithm();
  }

  std::pair<Points, Points> getBestInlierPoints() {
    Points pts1Inliers, pts2Inliers;
    s3d::copy_if_true(std::begin(pts1_), std::end(pts1_), back_inserter(pts1Inliers),
                      inliers_->getBest());
    s3d::copy_if_true(std::begin(pts2_), std::end(pts2_), back_inserter(pts2Inliers),
                      inliers_->getBest());
    return {pts1Inliers, pts2Inliers};
  }

 private:
  typename ModelSolver::ModelType runAlgorithm() {
    while (!trials_->reachedMaxNb()) {
      sampleModel();
      inliers_->updateCurrent(distances_, params_.distanceThreshold);
      inliers_->updateBest();
      trials_->updateNb(inliers_->getCurrentNb());
    }

    std::cout << inliers_->getBestNb() << std::endl;

    if (inliers_->getBestNb() < params_.minNbPts) {
      throw NotEnoughInliersFound(params_.minNbPts, inliers_->getBestNb());
    }
    auto bestInlierPoints = getBestInlierPoints();
    return ModelSolver::ComputeModel(bestInlierPoints.first, bestInlierPoints.second);
  }

  void sampleModel() {
    // todo: optimize by reusing memory
    auto samples = getUniformPointsSamples();
    auto&& model = ModelSolver::ComputeModel(samples.first, samples.second);
    DistanceFunction::ComputeDistance(pts1_, pts2_, model, &distances_);
    assert(distances_.size() == pts1_.size());
  }

  std::pair<Points, Points> getUniformPointsSamples() {
    auto randIndices =
        s3d::rand_n_unique_values(0, static_cast<int>(pts1_.size()) - 1, params_.minNbPts);
    auto pts1Sample = s3d::values_from_indices(pts1_, randIndices);
    auto pts2Sample = s3d::values_from_indices(pts2_, randIndices);
    return {pts1Sample, pts2Sample};
  };

  Params params_;
  Points pts1_{};
  Points pts2_{};
  Distances distances_{};
  std::unique_ptr<Trials> trials_{};
  std::unique_ptr<Inliers> inliers_{};
};

#endif  // S3D_ROBUST_ESTIMATION_RANSAC_H
