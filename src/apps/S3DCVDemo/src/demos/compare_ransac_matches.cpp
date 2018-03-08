/**
 * Compares s3d feature matching with a reference dataset
 */

#include "dataset_loader_vsg.h"
#include "demo_utils.h"

#include <s3d/cv/features/match_finder_cv.h>
#include <s3d/cv/rectification/rectifier_cv.h>
#include <s3d/cv/utilities/cv.h>
#include <s3d/math.h>
#include <s3d/utilities/eigen.h>

#include <opencv/cxeigen.hpp>
#include <opencv2/opencv.hpp>

#include <s3d/cv/disparity/disparity_analyzer_stan.h>
#include <s3d/cv/features/match_finder_cv.h>
#include <string>

int main(int argc, char* argv[]) {
  DatasetLoaderVSG dataset = createDatasetLoader(argc, argv);

  cv::Mat imgLeft = dataset.loadImageLeft();
  cv::Mat imgRight = dataset.loadImageRight();

  // find matches
  s3d::MatchFinderCV matchFinder;
  std::vector<std::vector<Eigen::Vector2d>> matches = matchFinder.findMatches(imgLeft, imgRight);

  // to homogeneous 2D
  std::vector<Eigen::Vector3d> ptsLeftH, ptsRightH;
  s3d::toHomogeneous2D(matches[0], &ptsLeftH);
  s3d::toHomogeneous2D(matches[1], &ptsRightH);

  // solve F with RANSAC
  s3d::robust::Parameters params;
  params.nbTrials = 500;
  params.distanceThreshold = 0.01;
  auto ransac = s3d::DisparityAnalyzerSTAN::RansacAlgorithmSTAN(params);

  s3d::StanAlignment alignment;
  try {
    alignment = ransac(ptsLeftH, ptsRightH);
  } catch (const s3d::robust::NotEnoughInliersFound& /*exception*/) {
    return -1;
  }

  // filter inliers
  std::vector<Eigen::Vector3d> bestPts1h, bestPts2h;
  std::tie(bestPts1h, bestPts2h) = ransac.getBestInlierSamples();
  std::vector<Eigen::Vector2d> inliersLeft, inliersRight;
  s3d::toEuclidian2DTruncate(bestPts1h, &inliersLeft);
  s3d::toEuclidian2DTruncate(bestPts2h, &inliersRight);

  // display dataset matches
  s3d::displayMatches("Feature Matches (Dataset)",
                      imgLeft,
                      imgRight,
                      dataset.loadPointsLeft(),
                      dataset.loadPointsRight());

  // display found matches
  s3d::displayMatches("Feature Matches (Before Ransac)", imgLeft, imgRight, matches[0], matches[1]);

  // display found matches
  s3d::displayMatches("Feature Matches (After Ransac)", imgLeft, imgRight, inliersLeft, inliersRight);

  cv::waitKey(0);
}
