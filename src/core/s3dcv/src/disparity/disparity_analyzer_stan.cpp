#include "s3d/cv/disparity/disparity_analyzer_stan.h"

#include <s3d/cv/features/match_finder_cv.h>
#include <s3d/cv/utilities/cv.h>
#include <s3d/disparity/utilities.h>
#include <s3d/utilities/histogram.h>

#include <opencv2/opencv.hpp>

namespace {
// todo: maybe this could be somewhere else to be reusable
void toHomogeneous2D(const std::vector<Eigen::Vector2d>& in, std::vector<Eigen::Vector3d>* result) {
  result->resize(in.size());
  std::transform(
      std::begin(in), std::end(in), std::begin(*result), [](const Eigen::Vector2d& value) {
        return Eigen::Vector3d(value.x(), value.y(), 1.0);
      });
}

// usually have to divide by z() but not necessary here
void toEuclidian2DTruncate(const std::vector<Eigen::Vector3d>& in,
                           std::vector<Eigen::Vector2d>* result) {
  result->resize(in.size());
  std::transform(
      std::begin(in), std::end(in), std::begin(*result), [](const Eigen::Vector3d& value) {
        return Eigen::Vector2d(value.x(), value.y());
      });
}
}  // namespace

namespace s3d {

DisparityAnalyzerSTAN::Results::Results() = default;

DisparityAnalyzerSTAN::Results::Results(double smoothingFactor)
    : minDisparityPercent{0.0, smoothingFactor},
      maxDisparityPercent{0.0, smoothingFactor},
      roll{0.0, smoothingFactor},
      vertical{0.0, smoothingFactor},
      panKeystone{0.0, smoothingFactor},
      tiltKeystone{0.0, smoothingFactor},
      tiltOffset{0.0, smoothingFactor},
      zoom{0.0, smoothingFactor} {}

void DisparityAnalyzerSTAN::Results::updateParameters(double minDisparity,
                                                      double maxDisparity,
                                                      float widthRatio,
                                                      RansacAlgorithmSTAN::ModelType model) {
  auto minDisparityPercent = minDisparity * widthRatio;
  auto maxDisparityPercent = maxDisparity * widthRatio;
  this->minDisparityPercent.addToAverage(minDisparityPercent);
  this->maxDisparityPercent.addToAverage(maxDisparityPercent);
  vertical.addToAverage(model.ch_y);
  roll.addToAverage(model.a_z);
  zoom.addToAverage(model.a_f);
  tiltOffset.addToAverage(model.f_a_x);
  panKeystone.addToAverage(model.a_x_f);
  tiltKeystone.addToAverage(model.a_y_f);
}

void DisparityAnalyzerSTAN::Results::updatePoints(const std::vector<Eigen::Vector2d>& bestPts,
                                                  std::vector<double> disparities,
                                                  float widthRatio,
                                                  float resizeRatio) {
  featurePoints.clear();
  disparitiesPercent.clear();
  for (int i = 0; i < bestPts.size(); ++i) {
    // filter out extreme percentiles
    if (minDisparityPercent / widthRatio <= disparities[i] &&
        disparities[i] <= maxDisparityPercent / widthRatio) {
      featurePoints.emplace_back(bestPts[i].x() * resizeRatio, bestPts[i].y() * resizeRatio);
      disparitiesPercent.push_back(static_cast<float>(disparities[i] * widthRatio));
    }
  }
}

void DisparityAnalyzerSTAN::Results::setSmoothingFactor(double smoothingFactor) {
  minDisparityPercent.setSmoothingFactor(smoothingFactor);
  maxDisparityPercent.setSmoothingFactor(smoothingFactor);
  roll.setSmoothingFactor(smoothingFactor);
  vertical.setSmoothingFactor(smoothingFactor);
  panKeystone.setSmoothingFactor(smoothingFactor);
  tiltKeystone.setSmoothingFactor(smoothingFactor);
  tiltOffset.setSmoothingFactor(smoothingFactor);
  zoom.setSmoothingFactor(smoothingFactor);
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
  constexpr float resizeRatio = 2.0f;
  resizeMat(&leftOrig, 1.0f / resizeRatio);
  resizeMat(&rightOrig, 1.0f / resizeRatio);

  // find matches
  auto matches = findMatches(leftOrig, rightOrig);
  if (not enoughMatches(matches[0].size())) {
    return false;
  }

  // to homogeneous
  std::vector<Eigen::Vector3d> pts1h, pts2h;
  toHomogeneous2D(matches[0], &pts1h);
  toHomogeneous2D(matches[1], &pts2h);

  // center points for ransac
  auto imageCenter = Eigen::Vector3d(leftOrig.rows / 2.0, leftOrig.cols / 2.0, 0.0);
  s3d::center_values(std::begin(pts1h), std::end(pts1h), std::begin(pts1h), imageCenter);
  s3d::center_values(std::begin(pts2h), std::end(pts2h), std::begin(pts2h), imageCenter);

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
  s3d::center_values(
      std::begin(bestPts1h), std::end(bestPts1h), std::begin(bestPts1h), -imageCenter);
  s3d::center_values(
      std::begin(bestPts2h), std::end(bestPts2h), std::begin(bestPts2h), -imageCenter);

  // compute disparity range
  std::vector<Eigen::Vector2d> bestPts1, bestPts2;
  toEuclidian2DTruncate(bestPts1h, &bestPts1);
  toEuclidian2DTruncate(bestPts2h, &bestPts2);
  std::vector<double> disparities;
  s3d::compute_disparities(bestPts1, bestPts2, back_inserter(disparities));
  double minDisparity, maxDisparity;
  std::tie(minDisparity, maxDisparity) = s3d::disparity_range(disparities);

  // Set outputs (moving average of smoothingFactor_)
  const float widthRatio = 100.0f / static_cast<float>(leftOrig.cols);
  results.updateParameters(minDisparity, maxDisparity, widthRatio, model);
  results.updatePoints(bestPts2, disparities, widthRatio, resizeRatio);

  return true;
}

const std::vector<float>& DisparityAnalyzerSTAN::getDisparitiesPercent() const {
  return results.disparitiesPercent;
}
const std::vector<Eigen::Vector2f>& DisparityAnalyzerSTAN::getFeaturePoints() const {
  return results.featurePoints;
}

void DisparityAnalyzerSTAN::setSmoothingFactor(double smoothingFactor) {
  results.setSmoothingFactor(smoothingFactor);
}

s3d::MatchFinder::Matches DisparityAnalyzerSTAN::findMatches(const cv::Mat& left,
                                                             const cv::Mat& right) {
  // find matches
  std::unique_ptr<s3d::MatchFinder> matchFinder = std::make_unique<s3d::MatchFinderCV>();
  auto matches = matchFinder->findMatches({s3d::cv2image(left), s3d::cv2image(right)});
  assert(matches.size() == 2 && matches[0].size() == matches[1].size());

  return matches;
}

bool DisparityAnalyzerSTAN::enoughMatches(size_t nbOfMatches) {
  return nbOfMatches >=
         s3d::robust_solver_traits<s3d::StanFundamentalMatrixSolver>::MIN_NB_SAMPLES * 3;
}

DisparityAnalyzerSTAN::RansacAlgorithmSTAN DisparityAnalyzerSTAN::createRansac(Size imageSize) {
  int h = imageSize.getHeight();
  int w = imageSize.getWidth();

  s3d::Ransac::Params params;
  params.nbTrials = 500;
  params.distanceThreshold = 0.01 * sqrt(h * h + w * w);

  return DisparityAnalyzerSTAN::RansacAlgorithmSTAN(params);
}

}  // namespace s3d
