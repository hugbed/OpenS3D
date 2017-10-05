#ifndef S3D_CV_DISPARITY_DISPARITY_ANALYZER_STAN_H
#define S3D_CV_DISPARITY_DISPARITY_ANALYZER_STAN_H

#include "s3d/disparity/disparity_analyzer.h"

#include "s3d/features/match_finder.h"
#include "s3d/multiview/sampson_distance_function.h"
#include "s3d/multiview/stan_fundamental_matrix_solver.h"
#include "s3d/robust_estimation/ransac.h"
#include "s3d/utilities/math.h"

#include <Eigen/Dense>

#include <opencv2/core/types.hpp>

#include <gsl/gsl>

namespace s3d {
class StanAlignment;

class DisparityAnalyzerSTAN : public DisparityAnalyzer {
 public:
  using RansacAlgorithmSTAN =
      s3d::RansacAlgorithm<s3d::StanFundamentalMatrixSolver, s3d::SampsonDistanceFunction>;

  struct Results {
    explicit Results();
    explicit Results(double smoothingFactor);

    void updateParameters(double minDisparity,
                          double maxDisparity,
                          float widthRatio,
                          RansacAlgorithmSTAN::ModelType model);

    void updatePoints(const std::vector<Eigen::Vector2d>& bestPtsLeft,
                      const std::vector<Eigen::Vector2d>& bestPtsRight,
                      std::vector<double> disparities,
                      float widthRatio,
                      float resizeRatio);

    void setSmoothingFactor(double smoothingFactor);

    StanAlignment getStanAlignment() const;

    s3d::MovingAverage<double> minDisparityPercent{};
    s3d::MovingAverage<double> maxDisparityPercent{};
    s3d::MovingAverage<double> vertical{};
    s3d::MovingAverage<double> roll{};
    s3d::MovingAverage<double> zoom{};
    s3d::MovingAverage<double> tiltOffset{};
    s3d::MovingAverage<double> panKeystone{};
    s3d::MovingAverage<double> tiltKeystone{};
    s3d::MovingAverage<double> zParallaxDeformation{};
    std::vector<Eigen::Vector2f> featurePointsLeft{};
    std::vector<Eigen::Vector2f> featurePointsRight{};

    std::vector<float> disparitiesPercent{};
  };

  DisparityAnalyzerSTAN();
  explicit DisparityAnalyzerSTAN(double smoothingFactor);

  gsl::owner<DisparityAnalyzerSTAN*> clone() const override;

  bool analyze(const cv::Mat& left, const cv::Mat& right);

  bool analyze(const Image<uint8_t>& left, const Image<uint8_t>& right) override;
  const std::vector<float>& getDisparitiesPercent() const override;
  const std::vector<Eigen::Vector2f>& getFeaturePointsLeft() const override;
  const std::vector<Eigen::Vector2f>& getFeaturePointsRight() const override;
  void setSmoothingFactor(double smoothingFactor);

  // outputs
  Results results;

 private:
  MatchFinder::Matches findMatches(const cv::Mat& left, const cv::Mat& right);

  bool enoughMatches(size_t nbOfMatches);

  RansacAlgorithmSTAN createRansac(Size imageSize);
};
}  // namespace s3d

#endif  // S3D_CV_DISPARITY_DISPARITY_ANALYZER_STAN_H
