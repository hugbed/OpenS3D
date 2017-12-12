#ifndef S3D_ROBUST_MODEL_SAMPLER_H
#define S3D_ROBUST_MODEL_SAMPLER_H

#include "s3d/robust/estimation_algorithm_traits.h"

#include "s3d/utilities/containers.h"
#include "s3d/utilities/rand.h"

namespace s3d::robust {

template<class ModelSolver, class DistanceFunction>
struct ModelSampler {
  using SampleType = typename estimation_algorithm_traits<ModelSolver>::SampleType;
  using ModelType = typename estimation_algorithm_traits<ModelSolver>::ModelType;
  using Samples = std::vector<SampleType>;

  ModelSampler(Samples samples1, Samples samples2)
    : samples1_{std::move(samples1)}
    , samples2_{std::move(samples2)}
  {
    distances_.resize(samples1_.size());
  }

  ModelType sampleModel() {
    // todo: optimize by reusing memory
    auto samples = getUniformSamples();
    auto model = ModelSolver::ComputeModel(samples.first, samples.second);
    DistanceFunction::ComputeDistance(samples1_, samples2_, model, &distances_);
    assert(distances_.size() == samples1_.size());
    return model;
  }

  std::pair<Samples, Samples> getSamplesWhereTrue(const std::vector<bool>& flags) {
    Samples inliers1, inliers2;
    s3d::copy_if_true(
            std::begin(samples1_), std::end(samples1_), back_inserter(inliers1), flags);
    s3d::copy_if_true(
            std::begin(samples2_), std::end(samples2_), back_inserter(inliers2), flags);
    return {inliers1, inliers2};
  };

  std::pair<Samples, Samples> getUniformSamples() {
    auto randIndices =
            s3d::rand_n_unique_values(0, static_cast<int>(samples1_.size()) - 1,
                                      estimation_algorithm_traits<ModelSolver>::MIN_NB_SAMPLES);
    auto samples1 = s3d::values_from_indices(samples1_, randIndices);
    auto samples2 = s3d::values_from_indices(samples2_, randIndices);
    return {samples1, samples2};
  };

  size_t nbSamples() { return samples1_.size(); }

  Samples samples1_{};
  Samples samples2_{};
  std::vector<double> distances_;
};

} // namespace s3d::robust

#endif //S3D_ROBUST_MODEL_SAMPLER_H
