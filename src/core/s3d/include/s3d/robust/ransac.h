#ifndef S3D_ROBUST_ESTIMATION_RANSAC_H
#define S3D_ROBUST_ESTIMATION_RANSAC_H

#include "s3d/robust/estimation_algorithm_traits.h"
#include "s3d/robust/parameters.h"
#include "s3d/robust/inliers.h"
#include "s3d/robust/trials.h"

#include "s3d/utilities/containers.h"
#include "s3d/utilities/rand.h"
#include "model_sampler.h"

#include <cassert>
#include <memory>
#include <stdexcept>

namespace s3d {
namespace robust {

class NotEnoughInliersFound : std::runtime_error {
public:
  NotEnoughInliersFound(size_t nbInliers, size_t expectedNb)
          : std::runtime_error(std::string("Not enough inliers found: [expected, found] = [") +
                               std::to_string(nbInliers) + std::string(", ") +
                               std::to_string(expectedNb) + std::string("]")) {}
};

/**
 * Robust estimation of a Model using random sampling
 * and an inlier/outlier classification strategy (RANSAC, LMedS...).
 */
template<class ModelSolver, class DistanceFunction>
class Ransac {
public:
  // retrieve traits
  using SampleType = typename estimation_algorithm_traits<ModelSolver>::SampleType;
  using ModelType = typename estimation_algorithm_traits<ModelSolver>::ModelType;
  static constexpr auto MIN_NB_SAMPLES = estimation_algorithm_traits<ModelSolver>::MIN_NB_SAMPLES;
  using Sampler = ModelSampler<ModelSolver, DistanceFunction>;

  using Samples = std::vector<SampleType>;

  explicit Ransac(Parameters params) : params_(params) {}

  ModelType operator()(Samples samples1, Samples samples2) {
    assert(samples1.size() == samples2.size());
    assert(samples1.size() >= MIN_NB_SAMPLES);

    modelSampler_ = std::make_unique<Sampler>(std::move(samples1), std::move(samples2));

    trials_ = std::make_unique<Trials>(modelSampler_->nbSamples(),
                                       estimation_algorithm_traits<ModelSolver>::MIN_NB_SAMPLES,
                                       params_.nbTrials,
                                       params_);
    inliers_ = std::make_unique<Inliers>(modelSampler_->nbSamples(), params_.distanceThreshold);

    return runAlgorithm();
  }

  std::pair<Samples, Samples> getBestInlierSamples() {
    return modelSampler_->getSamplesWhereTrue(inliers_->getBest());
  }

  size_t getTotalNumberOfIterations() {
    return trials_ != nullptr ? trials_->currentNb() : 0;
  }

private:
  ModelType runAlgorithm() {
    while (!trials_->reachedMaxNb()) {
      modelSampler_->sampleModel();
      inliers_->updateCurrent(modelSampler_->distances_);
      inliers_->updateBest();
      trials_->updateNb(inliers_->getCurrentNb());
    }

    if (inliers_->getBestNb() < MIN_NB_SAMPLES) {
      throw NotEnoughInliersFound(MIN_NB_SAMPLES, inliers_->getBestNb());
    }
    auto bestInlierPoints = getBestInlierSamples();
    return ModelSolver::ComputeModel(bestInlierPoints.first, bestInlierPoints.second);
  }

  Parameters params_;
  std::unique_ptr<Sampler> modelSampler_{};
  std::unique_ptr<Trials> trials_{};
  std::unique_ptr<Inliers> inliers_{};
};

}  // namespace robust
}  // namespace s3d

#endif  // S3D_ROBUST_ESTIMATION_RANSAC_H
