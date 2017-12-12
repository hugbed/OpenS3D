#ifndef S3D_ROBUST_LMEDS_H
#define S3D_ROBUST_LMEDS_H

#include "s3d/robust/estimation_algorithm_traits.h"
#include "s3d/robust/parameters.h"
#include "s3d/robust/inliers.h"
#include "s3d/robust/trials.h"

#include "s3d/utilities/containers.h"
#include "s3d/utilities/rand.h"

#include <cassert>
#include <memory>
#include <stdexcept>

namespace s3d {
namespace robust {

/**
 * Robust estimation of a Model using random sampling
 * and an inlier/outlier classification strategy (RANSAC, LMedS...).
 */
template<class ModelSolver, class DistanceFunction>
class Lmeds {
public:
  // retrieve traits
  using SampleType = typename estimation_algorithm_traits<ModelSolver>::SampleType;
  using ModelType = typename estimation_algorithm_traits<ModelSolver>::ModelType;
  static constexpr auto MIN_NB_SAMPLES = estimation_algorithm_traits<ModelSolver>::MIN_NB_SAMPLES;
  using Sampler = ModelSampler<ModelSolver, DistanceFunction>;

  using Samples = std::vector<SampleType>;

  explicit Lmeds(Parameters params) : params_(params) {}

  ModelType operator()(Samples samples1, Samples samples2) {
    assert(samples1.size() == samples2.size());
    assert(samples1.size() >= MIN_NB_SAMPLES);

    modelSampler_ = std::make_unique<Sampler>(std::move(samples1), std::move(samples2));

    trials_ = std::make_unique<Trials>(modelSampler_->nbSamples(),
                                       estimation_algorithm_traits<ModelSolver>::MIN_NB_SAMPLES,
                                       computeNbTrials(),
                                       params_);
    inliers_ = std::make_unique<InliersLMedS>(modelSampler_->nbSamples(), params_.distanceThreshold);

    return runAlgorithm();
  }

  std::pair<Samples, Samples> getBestInlierSamples() {
    return modelSampler_->getSamplesWhereTrue(bestInliers_);
  }


  size_t getTotalNumberOfIterations() {
    return trials_ != nullptr ? trials_->currentNb() : 0;
  }

private:
  ModelType runAlgorithm() {
    while (!trials_->reachedMaxNb()) {
      auto model = modelSampler_->sampleModel();
      inliers_->updateCurrent(modelSampler_->distances_);
      inliers_->updateBest();
      if (inliers_->wereLastBest()) {
        bestModel_ = model;
      }
    }

    // find inliers from bestModel
    DistanceFunction::ComputeDistance(modelSampler_->samples1_, modelSampler_->samples2_, bestModel_, &modelSampler_->distances_);
    size_t nbInliers;
    std::tie(bestInliers_, nbInliers) = inliers_->computeInliers(modelSampler_->distances_);

    if (nbInliers < estimation_algorithm_traits<ModelSolver>::MIN_NB_SAMPLES) {
      throw NotEnoughInliersFound(MIN_NB_SAMPLES, nbInliers);
    }

    return bestModel_;
  }

  size_t computeNbTrials() {
    size_t nbTrials = log(1.0 - params_.confidence) / log(1.0 - pow(0.5, MIN_NB_SAMPLES));
    nbTrials += sqrt(1 - pow(0.5, 4)) / pow(0.5, 4); // + std of multiples
    return std::min(nbTrials, params_.nbTrials);
  }

  Parameters params_;
  std::vector<bool> bestInliers_{};
  ModelType bestModel_{};
  std::unique_ptr<Sampler> modelSampler_;
  std::unique_ptr<Trials> trials_{};
  std::unique_ptr<Inliers> inliers_{};
};

}  // namespace robust
}  // namespace s3d

#endif // S3D_ROBUST_LMEDS_H
