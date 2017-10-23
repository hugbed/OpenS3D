/**
 * Compares s3d feature matching with a reference dataset
 */

#include "dataset_loader_vsg.h"
#include "demo_utils.h"

#include <s3d/cv/disparity/disparity_analyzer_stan.h>
#include <s3d/cv/features/match_finder_cv.h>
#include <s3d/cv/rectification/rectifier_cv.h>
#include <s3d/math.h>

int main(int argc, char* argv[]) {
  DatasetLoaderVSG dataset = createDatasetLoader(argc, argv);

  cv::Mat imgLeft = dataset.loadImageLeft();
  cv::Mat imgRight = dataset.loadImageRight();

  // load points to homogeneous coordinates
  std::vector<Eigen::Vector2d> ptsLeft = dataset.loadPointsLeft(),
                               ptsRight = dataset.loadPointsRight();
  std::vector<Eigen::Vector3d> ptsLeftH, ptsRightH;
  s3d::toHomogeneous2D(dataset.loadPointsLeft(), &ptsLeftH);
  s3d::toHomogeneous2D(dataset.loadPointsRight(), &ptsRightH);

  // display matches
  s3d::displayMatches("Feature Matches (After Ransac)", imgLeft, imgRight, ptsLeft, ptsRight);

  // compute fundamental matrix
  s3d::StanAlignment alignment =
      s3d::StanFundamentalMatrixSolver::ComputeModel(ptsLeftH, ptsRightH);
  Eigen::Matrix3d F = s3d::StanFundamentalMatrixSolver::FundamentalMatrixFromAlignment(alignment);

  // draw epipolar lines
  cv::Mat imgLeftEpilines, imgRightEpilines;
  std::tie(imgLeftEpilines, imgRightEpilines) =
      s3d::drawEpipolarLines(s3d::eigenMatToCV(F),
                             imgLeft,
                             imgRight,
                             s3d::eigenPointsToCV(ptsLeft),
                             s3d::eigenPointsToCV(ptsRight));

  cv::Mat imgEpilines(imgLeftEpilines.rows, imgLeftEpilines.cols * 2, CV_8UC3);
  cv::hconcat(imgLeftEpilines, imgRightEpilines, imgEpilines);
  cv::imshow("Epilines From Dataset and STAN Solver", imgEpilines);

  // display model rectification
  Eigen::Matrix3f H1 = s3d::RectificationStan::leftImageMatrix(alignment);
  Eigen::Matrix3f H2 = s3d::RectificationStan::rightImageMatrix(alignment);

  s3d::RectifierCV rectifier;
  cv::Mat imgLeftRect = rectifier.rectifyCV(imgLeftEpilines, s3d::eigenMatToCV(H1));
  cv::Mat imgRightRect = rectifier.rectifyCV(imgRightEpilines, s3d::eigenMatToCV(H2));

  cv::Mat imgRect(imgLeftRect.rows, imgLeftRect.cols * 2, CV_8UC3);
  cv::hconcat(imgLeftRect, imgRightRect, imgRect);
  cv::imshow("Rectified Pairs With Epilines", imgRect);

  cv::waitKey(0);
}
