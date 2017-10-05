#ifndef S3D_ROBUST_ESTIMATION_RANSAC_H
#define S3D_ROBUST_ESTIMATION_RANSAC_H

#include "s3d/robust_estimation/robust_estimation_traits.h"

#include "s3d/utilities/containers.h"
#include "s3d/utilities/rand.h"

#include <cassert>
#include <memory>
#include <stdexcept>

namespace s3d {

// todo: put this in some more restricted scope (namespace robust?)
class NotEnoughInliersFound : std::runtime_error {
 public:
  NotEnoughInliersFound(size_t nbInliers, size_t expectedNb)
      : std::runtime_error(std::string("Not enough inliers found: [expected, found] = [") +
                           std::to_string(nbInliers) + std::string(", ") +
                           std::to_string(expectedNb) + std::string("]")) {}
};

// Empty base class for common types
class Ransac {
 public:
  using Distances = std::vector<double>;

  struct Params {
    size_t nbTrials{500};
    double distanceThreshold{0.01};
    double confidence{0.999};
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
  // retrieve traits
  using SampleType = typename robust_solver_traits<ModelSolver>::SampleType;
  using ModelType = typename robust_solver_traits<ModelSolver>::ModelType;
  static constexpr auto MIN_NB_SAMPLES = robust_solver_traits<ModelSolver>::MIN_NB_SAMPLES;

  using Samples = std::vector<SampleType>;

  explicit RansacAlgorithm(Params params) : params_(params) {}

  ModelType operator()(const Samples& samples1, const Samples& samples2) {
    assert(samples1.size() == samples2.size());
    assert(samples1.size() >= MIN_NB_SAMPLES);

    samples1_ = std::move(samples1);
    samples2_ = std::move(samples2);
    trials_ = std::make_unique<Trials>(samples1_.size(), params_.nbTrials, params_);
    inliers_ = std::make_unique<Inliers>(samples1_.size());
    distances_.resize(samples1_.size());

    return runAlgorithm();
  }

  std::pair<Samples, Samples> getBestInlierSamples() {
    Samples inliers1, inliers2;
    s3d::copy_if_true(
        std::begin(samples1_), std::end(samples1_), back_inserter(inliers1), inliers_->getBest());
    s3d::copy_if_true(
        std::begin(samples2_), std::end(samples2_), back_inserter(inliers2), inliers_->getBest());
    return {inliers1, inliers2};
  }

 private:
  ModelType runAlgorithm() {
    while (!trials_->reachedMaxNb()) {
      sampleModel();
      inliers_->updateCurrent(distances_, params_.distanceThreshold);
      inliers_->updateBest();
      trials_->updateNb(inliers_->getCurrentNb());
    }

    if (inliers_->getBestNb() < MIN_NB_SAMPLES) {
      throw NotEnoughInliersFound(MIN_NB_SAMPLES, inliers_->getBestNb());
    }
    auto bestInlierPoints = getBestInlierSamples();
    return ModelSolver::ComputeModel(bestInlierPoints.first, bestInlierPoints.second);
  }

  void sampleModel() {
    // todo: optimize by reusing memory
    auto samples = getUniformSamples();
    auto&& model = ModelSolver::ComputeModel(samples.first, samples.second);
    DistanceFunction::ComputeDistance(samples1_, samples2_, model, &distances_);
    assert(distances_.size() == samples1_.size());
  }

  std::pair<Samples, Samples> getUniformSamples() {
    auto randIndices =
        s3d::rand_n_unique_values(0, static_cast<int>(samples1_.size()) - 1, MIN_NB_SAMPLES);
    auto samples1 = s3d::values_from_indices(samples1_, randIndices);
    auto samples2 = s3d::values_from_indices(samples2_, randIndices);
    return {samples1, samples2};
  };

  Params params_;
  Samples samples1_{};
  Samples samples2_{};
  Distances distances_{};
  std::unique_ptr<Trials> trials_{};
  std::unique_ptr<Inliers> inliers_{};
};

}  // namespace s3d

#endif  // S3D_ROBUST_ESTIMATION_RANSAC_H
