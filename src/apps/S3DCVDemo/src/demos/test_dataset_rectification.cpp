/**
 * Compares s3d feature matching with a reference dataset
 */

#include "dataset_loader_vsg.h"
#include "demo_utils.h"

#include <s3d/cv/rectification/rectifier_cv.h>
#include <s3d/cv/utilities/cv.h>
#include <s3d/utilities/eigen.h>

#include <opencv/cxeigen.hpp>
#include <opencv2/opencv.hpp>

#include <string>

int main(int argc, char* argv[]) {
  DatasetLoaderVSG dataset = createDatasetLoader(argc, argv);

  cv::Mat imgLeft = dataset.loadImageLeft();
  cv::Mat imgRight = dataset.loadImageRight();

  // display dataset matches
  s3d::displayMatches(
      "Feature Matches", imgLeft, imgRight, dataset.loadPointsLeft(), dataset.loadPointsRight());

  // display dataset epipolar lines
  std::vector<Eigen::Vector2d> ptsLeft = dataset.loadPointsLeft();
  std::vector<Eigen::Vector2d> ptsRight = dataset.loadPointsRight();
  Eigen::Matrix3f F = dataset.loadFundamentalMatrix();

  auto ptsLeftCV = s3d::eigenPointsToCV(ptsLeft);
  auto ptsRightCV = s3d::eigenPointsToCV(ptsRight);
  auto F_CV = s3d::eigenMatToCV(F);
  s3d::displayEpipolarLines("Epipolar Lines", F_CV, imgLeft, imgRight, ptsLeftCV, ptsRightCV);

  // rectification using dataset H1, H2
  Eigen::Matrix3f H1 = dataset.loadRectificationMatrixLeft();
  Eigen::Matrix3f H2 = dataset.loadRectificationMatrixRight();

  // rectify images with epilines to see if they become parallel
  cv::Mat imgLeftEpilines, imgRightEpilines;
  std::tie(imgLeftEpilines, imgRightEpilines) =
      s3d::drawEpipolarLines(F_CV, imgLeft, imgRight, ptsLeftCV, ptsRightCV);

  // rectify using a rectifier
  s3d::RectifierCV rectifier;
  cv::Mat imgLeftRect = rectifier.rectifyCV(imgLeftEpilines, s3d::eigenMatToCV(H1));
  cv::Mat imgRightRect = rectifier.rectifyCV(imgRightEpilines, s3d::eigenMatToCV(H2));

  // display rectified images with epilines
  cv::Mat imgRect(imgLeftRect.rows, imgLeftRect.cols * 2, CV_8UC3);
  cv::hconcat(imgLeftRect, imgRightRect, imgRect);
  cv::imshow("Rectified Pairs With Epilines", imgRect);

  cv::waitKey(0);
}
