#ifndef S3D_ROBUST_ESTIMATION_RANSAC_H
#define S3D_ROBUST_ESTIMATION_RANSAC_H

namespace s3d {

template <class Container, class T>
bool contains(Container c, T t) {
  return (std::find(std::begin(c), std::end(c), t) != std::end(c));
}

template <class InIt, class OutIt>
void copy_if_true(InIt srcBegin, InIt srcEnd, OutIt dstBegin, const std::vector<bool>& flags) {
  auto i = 0;
  std::copy_if(srcBegin, srcEnd, dstBegin, [&i, flags](typename InIt::value_type) {
    return flags[i++];
  });
}

template <class SizeType>
std::vector<int> rand_n_unique_values(int minVal, int maxVal, SizeType n, int seed) {
  assert(minVal < maxVal);

  std::mt19937 mt(seed);
  std::uniform_int_distribution<int> dist(minVal, maxVal);

  std::vector<int> randValues;
  randValues.reserve(n);

  for (auto i = 0ULL; i < n; ++i) {
    auto randNb = dist(mt);
    while (s3d::contains(randValues, randNb)) {
      randNb = dist(mt);
    }
    randValues.emplace_back(randNb);
  }
  return randValues;
};

template <class SizeType>
std::vector<int> rand_n_unique_values(int minVal, int maxVal, SizeType n) {
  std::random_device rd;
  return rand_n_unique_values(minVal, maxVal, n, rd());
};

template <class T>
std::vector<T> values_from_indices(std::vector<T> values, std::vector<int> indices) {
  std::vector<T> sample;
  sample.reserve(indices.size());

  for (auto i : indices) {
    assert(i < values.size());
    sample.emplace_back(values[i]);
  }

  return sample;
};

}  // namespace s3d

template <class ModelSolver, class DistanceFunction>
class Ransac {
 public:
  struct Params {
    size_t nbTrials{500};
    double distanceThreshold{0.01};
    double confidence{0.999};
    size_t minNbPts{};
  };

  using Points = std::vector<typename ModelSolver::PointsType>;
  using Distances = std::vector<double>;

  explicit Ransac(Ransac::Params params) : params_(params) {}

  typename ModelSolver::ModelType operator()(Points pts1, Points pts2) {
    assert(pts1.size() == pts2.size());
    assert(pts1.size() >= params_.minNbPts);

    pts1_ = std::move(pts1);
    pts2_ = std::move(pts2);
    state_ = std::make_unique<State>(pts1_.size(), params_.nbTrials, params_);

    return ransacAlg();
  }

 private:
  // todo: this may be doing too much stuff (inliers, nbTrials, distances...)
  struct State {
    explicit State(size_t nbPts, size_t maxNbTrials, Params params)
        : params_(params),
          maxNbTrials{maxNbTrials},
          logOneMinusConf{log(1.0 - params.confidence)},
          oneOverNbPts{1.0 / nbPts} {
      currentInliers.resize(nbPts);
      bestInliers.resize(nbPts);
      distances.resize(nbPts);
    }

    bool currentInliersAreBetter() { return currentNbInliers > bestNbInliers; }

    void chooseCurrentInliersAsBest() {
      bestNbInliers = currentNbInliers;
      std::copy(std::begin(currentInliers), std::end(currentInliers), std::begin(bestInliers));
    }

    void updateBestInliers() {
      if (currentInliersAreBetter()) {
        chooseCurrentInliersAsBest();
      }
    }

    void updateNbTrials() {
      constexpr double eps = 1E-15;

      size_t newNb = 0;
      double ratioOfInliers = static_cast<double>(currentNbInliers) * oneOverNbPts;
      if (ratioOfInliers <= (1 - eps)) {
        auto ratio7 = std::pow(ratioOfInliers, 7);
        if (ratio7 > eps) {
          auto logOneMinusRatio7 = std::log(1 - ratio7);
          newNb = size_t(std::ceil(logOneMinusConf / logOneMinusRatio7));
        } else {
          newNb = std::numeric_limits<size_t>::max();
        }
      }

      if (maxNbTrials > newNb) {
        maxNbTrials = newNb;
      }
    }

    void updateInliers() {
      currentInliers.clear();
      currentInliers.resize(distances.size());
      currentNbInliers = 0;
      for (auto i = 0ULL; i < distances.size(); ++i) {
        if (distances[i] < params_.distanceThreshold) {
          currentInliers[i] = true;
          currentNbInliers++;
        }
      }
    }

    bool reachedMaxNbTrials() {
      bool hasReached = curNbTrials >= maxNbTrials;
      curNbTrials++;
      return hasReached;
    }

    Params params_;

    size_t maxNbTrials{};
    size_t currentNbInliers{0};
    std::vector<bool> currentInliers;
    size_t bestNbInliers{0};
    std::vector<bool> bestInliers;
    Distances distances;

    size_t curNbTrials{0};

    const double logOneMinusConf;
    const double oneOverNbPts;
  };

  typename ModelSolver::ModelType ransacAlg() {
    while (!state_->reachedMaxNbTrials()) {
      sampleModel();
      state_->updateInliers();
      state_->updateBestInliers();
      state_->updateNbTrials();
    }
    // todo: ok, assert is too brutal. Exception would be better
    assert(state_->bestNbInliers >= params_.minNbPts);
    auto bestInliers = getBestInliers();
    return ModelSolver::ComputeModel(bestInliers.first, bestInliers.second);
  }

  void sampleModel() {
    // todo: optimize by reusing memory
    auto randIndices =
        s3d::rand_n_unique_values(0, static_cast<int>(pts1_.size()) - 1, params_.minNbPts);
    auto pts1Sample = s3d::values_from_indices(pts1_, randIndices);
    auto pts2Sample = s3d::values_from_indices(pts2_, randIndices);

    // something like that but reuse memory please
    auto&& model = ModelSolver::ComputeModel(pts1Sample, pts2Sample);
    DistanceFunction::ComputeDistance(pts1_, pts2_, model, &state_->distances);
    assert(state_->distances.size() == pts1_.size());
  }

  std::pair<Points, Points> getBestInliers() {
    Points pts1Inliers, pts2Inliers;
    s3d::copy_if_true(std::begin(pts1_), std::end(pts1_), back_inserter(pts1Inliers),
                      state_->bestInliers);
    s3d::copy_if_true(std::begin(pts2_), std::end(pts2_), back_inserter(pts2Inliers),
                      state_->bestInliers);
    return {pts1Inliers, pts2Inliers};
  }


  Params params_;
  Points pts1_{};
  Points pts2_{};
  std::unique_ptr<State> state_{};
};

#endif  // S3D_ROBUST_ESTIMATION_RANSAC_H
