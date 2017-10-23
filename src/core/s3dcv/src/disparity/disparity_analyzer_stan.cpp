#include "s3d/cv/disparity/disparity_analyzer_stan.h"

#include <s3d/cv/features/match_finder_cv.h>
#include <s3d/cv/utilities/cv.h>
#include <s3d/disparity/utilities.h>
#include <s3d/math.h>
#include <s3d/utilities/histogram.h>

#include <opencv2/opencv.hpp>

#include <cassert>

namespace s3d {

DisparityAnalyzerSTAN::Results::Results() = default;

DisparityAnalyzerSTAN::Results::Results(double smoothingFactor)
    : minDisparityPercent{0.0, smoothingFactor},
      maxDisparityPercent{0.0, smoothingFactor},
      vertical{0.0, smoothingFactor},
      roll{0.0, smoothingFactor},
      zoom{0.0, smoothingFactor},
      tiltOffset{0.0, smoothingFactor},
      tiltKeystone{0.0, smoothingFactor},
      panKeystone{0.0, smoothingFactor},
      zParallaxDeformation{0.0, smoothingFactor} {}

void DisparityAnalyzerSTAN::Results::updateParameters(double minDisparity,
                                                      double maxDisparity,
                                                      float widthRatio,
                                                      RansacAlgorithmSTAN::ModelType model) {
  auto minDisparityPercent = minDisparity * widthRatio;
  auto maxDisparityPercent = maxDisparity * widthRatio;
  this->minDisparityPercent.addToAverage(minDisparityPercent);
  this->maxDisparityPercent.addToAverage(maxDisparityPercent);
  vertical.addToAverage(model.ch_y);                // * 180 / PI (degrees)
  roll.addToAverage(model.a_z);                     // model.a_z * 180.0 / PI (degrees)
  zoom.addToAverage(model.a_f);                     // (model.a_f + 1.0) * 100.0 (%)
  tiltOffset.addToAverage(model.f_a_x);             // pixels
  tiltKeystone.addToAverage(model.a_y_f);           // radians / m
  panKeystone.addToAverage(model.a_x_f);            // radians / m
  zParallaxDeformation.addToAverage(model.ch_z_f);  // ratio (m/m)
}

void DisparityAnalyzerSTAN::Results::updatePoints(const std::vector<Eigen::Vector2d>& bestPtsLeft,
                                                  const std::vector<Eigen::Vector2d>& bestPtsRight,
                                                  std::vector<double> disparities,
                                                  float widthRatio,
                                                  float resizeRatio) {
  assert(bestPtsLeft.size() == bestPtsRight.size());

  featurePointsLeft.clear();
  featurePointsRight.clear();
  disparitiesPercent.clear();
  for (int i = 0; i < bestPtsLeft.size(); ++i) {
    // filter out extreme percentiles
    if (minDisparityPercent / widthRatio <= disparities[i] &&
        disparities[i] <= maxDisparityPercent / widthRatio) {
      featurePointsLeft.emplace_back(bestPtsLeft[i].x() * resizeRatio,
                                     bestPtsLeft[i].y() * resizeRatio);
      featurePointsRight.emplace_back(bestPtsRight[i].x() * resizeRatio,
                                      bestPtsRight[i].y() * resizeRatio);
      disparitiesPercent.push_back(static_cast<float>(disparities[i] * widthRatio));
    }
  }
}

void DisparityAnalyzerSTAN::Results::setSmoothingFactor(double smoothingFactor) {
  minDisparityPercent.setSmoothingFactor(smoothingFactor);
  maxDisparityPercent.setSmoothingFactor(smoothingFactor);
  vertical.setSmoothingFactor(smoothingFactor);
  roll.setSmoothingFactor(smoothingFactor);
  zoom.setSmoothingFactor(smoothingFactor);
  tiltOffset.setSmoothingFactor(smoothingFactor);
  tiltKeystone.setSmoothingFactor(smoothingFactor);
  panKeystone.setSmoothingFactor(smoothingFactor);
  zParallaxDeformation.setSmoothingFactor(smoothingFactor);
}

StanAlignment DisparityAnalyzerSTAN::Results::getStanAlignment() const {
  StanAlignment alignment;
  alignment.ch_y = static_cast<double>(vertical);
  alignment.a_z = static_cast<double>(roll);
  alignment.a_f = static_cast<double>(zoom);
  alignment.f_a_x = static_cast<double>(tiltOffset);
  alignment.a_y_f = static_cast<double>(tiltKeystone);
  alignment.a_x_f = static_cast<double>(panKeystone);
  alignment.ch_z_f = static_cast<double>(zParallaxDeformation);
  return alignment;
}

DisparityAnalyzerSTAN::DisparityAnalyzerSTAN() : results{} {}

DisparityAnalyzerSTAN::DisparityAnalyzerSTAN(double smoothingFactor) : results{smoothingFactor} {}

gsl::owner<DisparityAnalyzerSTAN*> DisparityAnalyzerSTAN::clone() const {
  return new DisparityAnalyzerSTAN(results.minDisparityPercent.getSmoothingFactor());
}

bool DisparityAnalyzerSTAN::analyze(const Image<uint8_t>& left, const Image<uint8_t>& right) {
  return analyze(image2cv(left), image2cv(right));
}

bool DisparityAnalyzerSTAN::analyze(const cv::Mat& leftImage, const cv::Mat& rightImage) {
  // copy images
  cv::Mat leftOrig = leftImage.clone();
  cv::Mat rightOrig = rightImage.clone();

  // resize images
  constexpr float resizeRatio = 1.0f;
  resizeMat(&leftOrig, 1.0f / resizeRatio);
  resizeMat(&rightOrig, 1.0f / resizeRatio);

  // find matches
  auto matches = findMatches(leftOrig, rightOrig);
  if (not enoughMatches(matches[0].size())) {
    return false;
  }

  // to homogeneous
  std::vector<Eigen::Vector3d> pts1h, pts2h;
  s3d::toHomogeneous2D(matches[0], &pts1h);
  s3d::toHomogeneous2D(matches[1], &pts2h);

  // solve F with RANSAC
  auto ransac = createRansac(Size(leftOrig.rows, leftOrig.cols));

  RansacAlgorithmSTAN::ModelType model;
  try {
    model = ransac(pts1h, pts2h);
  } catch (const s3d::NotEnoughInliersFound& /*exception*/) {
    return false;
  }

  // filter inliers
  std::vector<Eigen::Vector3d> bestPts1h, bestPts2h;
  std::tie(bestPts1h, bestPts2h) = ransac.getBestInlierSamples();

  // compute disparity range
  std::vector<Eigen::Vector2d> bestPts1, bestPts2;
  s3d::toEuclidian2DTruncate(bestPts1h, &bestPts1);
  s3d::toEuclidian2DTruncate(bestPts2h, &bestPts2);
  std::vector<double> disparities;
  s3d::compute_disparities(bestPts1, bestPts2, back_inserter(disparities));
  double minDisparity, maxDisparity;
  std::tie(minDisparity, maxDisparity) = s3d::disparity_range(disparities);

  // todo: for video, do not update outputs if not enough matches
  //       this should be done outside this class though or as a parameter
  //       i.e: updateResultsOnFewMatches, setMinimumNumberOfMatches
  //  if (enoughMatches(bestPts1.size()))
  //  {

  // Set outputs (moving average of smoothingFactor_)
  const float widthRatio = 100.0f / static_cast<float>(leftOrig.cols);
  results.updateParameters(minDisparity, maxDisparity, widthRatio, model);
  results.updatePoints(bestPts1, bestPts2, disparities, widthRatio, resizeRatio);

  //    return true;
  //  }

  return enoughMatches(results.featurePointsLeft.size());;
}

const std::vector<float>& DisparityAnalyzerSTAN::getDisparitiesPercent() const {
  return results.disparitiesPercent;
}

const std::vector<Eigen::Vector2f>& DisparityAnalyzerSTAN::getFeaturePointsLeft() const {
  return results.featurePointsLeft;
}

const std::vector<Eigen::Vector2f>& DisparityAnalyzerSTAN::getFeaturePointsRight() const {
  return results.featurePointsRight;
}

void DisparityAnalyzerSTAN::setSmoothingFactor(double smoothingFactor) {
  results.setSmoothingFactor(smoothingFactor);
}

// todo: why to I always recreate a matchFinder...
s3d::MatchFinder::Matches DisparityAnalyzerSTAN::findMatches(const cv::Mat& left,
                                                             const cv::Mat& right) {
  // find matches
  std::unique_ptr<s3d::MatchFinderCV> matchFinder = std::make_unique<s3d::MatchFinderCV>();
  auto matches = matchFinder->findMatches(left, right);
  assert(matches.size() == 2 && matches[0].size() == matches[1].size());

  return matches;
}

bool DisparityAnalyzerSTAN::enoughMatches(size_t nbOfMatches) {
  return nbOfMatches >=
         s3d::robust_solver_traits<s3d::StanFundamentalMatrixSolver>::MIN_NB_SAMPLES * 20;
}

DisparityAnalyzerSTAN::RansacAlgorithmSTAN DisparityAnalyzerSTAN::createRansac(Size imageSize) {
  int h = imageSize.getHeight();
  int w = imageSize.getWidth();

  s3d::Ransac::Params params;
  params.nbTrials = 500;
  params.distanceThreshold = 0.01;  // * sqrt(h * h + w * w);

  return DisparityAnalyzerSTAN::RansacAlgorithmSTAN(params);
}

}  // namespace s3d
