#ifndef S3D_CV_DISPARITY_DISPARITY_ANALYZER_STAN_H
#define S3D_CV_DISPARITY_DISPARITY_ANALYZER_STAN_H

#include "s3d/disparity/disparity_analyzer.h"

#include "s3d/cv/features/match_finder_surf.h"
#include "s3d/multiview/sampson_distance_function.h"
#include "s3d/multiview/stan_fundamental_matrix_solver.h"
#include "s3d/multiview/stan_results.h"
#include "s3d/robust/ransac.h"
//#include "s3d/robust/lmeds.h"
#include "s3d/utilities/math.h"

#include <Eigen/Dense>

#include <opencv2/core/types.hpp>

#include <gsl/gsl>

namespace s3d {
struct StanAlignment;
class MatchFinderCV;
class MatchFinderSurf;

class DisparityAnalyzerSTAN : public DisparityAnalyzer {
 public:
  // can alternatively use Lmeds
  using RansacAlgorithmSTAN =
      s3d::robust::Ransac<s3d::StanFundamentalMatrixSolver, s3d::SampsonDistanceFunction>;

  struct Results {
    explicit Results();

    void updateParameters(double minDisparity,
                          double maxDisparity,
                          float widthRatio,
                          RansacAlgorithmSTAN::ModelType model);

    void updatePoints(const std::vector<Eigen::Vector2d>& bestPtsLeft,
                      const std::vector<Eigen::Vector2d>& bestPtsRight,
                      std::vector<double> disparities,
                      float widthRatio,
                      float resizeRatio);

    StanResults stan;
    double minDisparityPercent{};
    double maxDisparityPercent{};

    std::vector<float> disparitiesPercent{};
  };

  DisparityAnalyzerSTAN();

  gsl::owner<DisparityAnalyzerSTAN*> clone() const override;

  bool analyze(const cv::Mat& left, const cv::Mat& right);

  bool analyze(const Image<uint8_t>& left, const Image<uint8_t>& right) override;
  const std::vector<float>& getDisparitiesPercent() const override;
  const std::vector<Eigen::Vector2f>& getFeaturePointsLeft() const override;
  const std::vector<Eigen::Vector2f>& getFeaturePointsRight() const override;
  void setMinimumNumberOfInliers(int minNbInliers);

  // outputs
  Results results;

 private:
  MatchFinder::Matches findMatches(const cv::Mat& left, const cv::Mat& right);
  bool enoughMatches(int nbOfMatches);
  RansacAlgorithmSTAN createRansac(Size imageSize);

  std::unique_ptr<MatchFinderCV> matchFinder_{std::make_unique<s3d::MatchFinderSurf>()};
  int minNbInliers_{4};
};
}  // namespace s3d

#endif  // S3D_CV_DISPARITY_DISPARITY_ANALYZER_STAN_H
