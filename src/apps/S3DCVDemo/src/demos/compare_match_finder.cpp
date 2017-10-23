/**
 * Compares s3d feature matching with a reference dataset
 */

#include "dataset_loader_vsg.h"
#include "demo_utils.h"

#include <s3d/cv/features/match_finder_cv.h>
#include <s3d/cv/rectification/rectifier_cv.h>
#include <s3d/cv/disparity/disparity_analyzer_stan.h>
#include <s3d/cv/utilities/cv.h>

#include <opencv/cxeigen.hpp>
#include <opencv2/opencv.hpp>

#include <Eigen/Dense>

#include <string>
#include <vector>

int main(int argc, char* argv[]) {
  DatasetLoaderVSG dataset = createDatasetLoader(argc, argv);

  cv::Mat imgLeft = dataset.loadImageLeft();
  cv::Mat imgRight = dataset.loadImageRight();

  // find matches with match finder
  s3d::MatchFinderCV matchFinder;
  std::vector<std::vector<Eigen::Vector2d>> matches = matchFinder.findMatches(imgLeft, imgRight);

  // display dataset matches
  s3d::displayMatches("Feature Matches (Dataset)",
                      imgLeft,
                      imgRight,
                      dataset.loadPointsLeft(),
                      dataset.loadPointsRight());

  // display found matches
  s3d::displayMatches("MatchFinder Matches", imgLeft, imgRight, matches[0], matches[1]);

  cv::waitKey(0);
}
