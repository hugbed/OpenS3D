#ifndef S3D_ROBUST_INLIERS_H
#define S3D_ROBUST_INLIERS_H

#include <limits>
#include <vector>
#include <cstddef> // size_t

namespace s3d {
namespace robust {

/**
 * Uses RANSAC strategy by default (distance < threshold)
 */
class Inliers {
public:
  explicit Inliers(size_t nbPts, double distanceThreshold);

  virtual bool currentInliersAreBetter() const;

  virtual void chooseCurrentInliersAsBest();

  virtual void updateBest();

  virtual void updateCurrent(const std::vector<double> &distances);

  virtual std::pair<std::vector<bool>, size_t> computeInliers(const std::vector<double> &distances);

  size_t getCurrentNb() const noexcept;

  size_t getBestNb() const noexcept;

  double getDistanceThreshold() const;

  const std::vector<bool> &getBest() const noexcept;

  bool wereLastBest();

private:
  size_t currentNb_{0};
  std::vector<bool> current_;
  size_t bestNb_{0};
  std::vector<bool> best_;
  bool lastWasBest_{false};

protected:
  virtual std::pair<std::vector<bool>, size_t> computeInliers(const std::vector<double> &distances, double distanceThreshold);

  double distanceThreshold_;
};

/**
 * Uses LMedS strategy (distance < median(distance)
 */
class InliersLMedS : public Inliers {
public :
  explicit InliersLMedS(size_t nbPts, double distanceThreshold);

  void updateCurrent(const std::vector<double> &distances) override;

  void chooseCurrentInliersAsBest() override;

  bool currentInliersAreBetter() const override;

  std::pair<std::vector<bool>, size_t> computeInliers(const std::vector<double> &distances) override;

private:
  double bestDist_{std::numeric_limits<double>::max()};
};

} // namespace robust
} // namespace s3d

#endif //S3D_ROBUST_INLIERS_H
