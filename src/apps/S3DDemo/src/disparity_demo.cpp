#include <iostream>

#include "opencv2/cudafeatures2d.hpp"
#include "opencv2/cudastereo.hpp"
#include "s3d/utilities/time.h"
#include <array>

#include "s3d/disparity/disparity_algorithm_bm.h"
#include "s3d/disparity/disparity_algorithm_orb.h"

using namespace s3d;

void displayInNewWindow(const std::string& name, cv::InputArray src) {
  cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
  cv::imshow(name, src);
  cv::waitKey(0);
}

int main(int argc, char* argv[]) {
  std::cout << "Hello, World!" << std::endl;

  assert(argc == 2);

  auto path_left = std::string(argv[1]);
  auto path_right = std::string(argv[1]);
  path_left.append("left.jpg");
  path_right.append("left.jpg");

  auto left = cv::imread(path_left, CV_LOAD_IMAGE_GRAYSCALE);
  auto right = cv::imread(path_right, CV_LOAD_IMAGE_GRAYSCALE);
  assert(left.data);
  assert(right.data);

  // debugging
  cv::Mat combined(left.rows, left.cols * 2, left.type());
  cv::hconcat(left, right, combined);
  displayInNewWindow("before", combined);

  // Compute disparities
  auto disparityAlgo = std::unique_ptr<DisparityAlgorithm>(
      std::make_unique<DisparityAlgorithmORB>());
  auto disparities =
      disparityAlgo->ComputeDisparities(Image{left}, Image{right});

  // Display disparity map
  cv::Mat cm_disp;
  auto disp = disparities->getDisparityMap();
  applyColorMap(disp.mat, cm_disp, cv::COLORMAP_OCEAN);
  displayInNewWindow("dispMap", cm_disp);

  // Display disparity points
  cv::Mat coloredMap(left.size(), CV_8UC3, cv::Scalar(0, 0, 0));
  cv::cvtColor(left, coloredMap, CV_GRAY2BGR);

  auto minVal = disparities->min().disparity.col;
  auto maxVal = disparities->max().disparity.col;

  auto disparityPoints = disparities->getDisparities();
  for (auto& d : disparityPoints) {
    //        auto rowColor = static_cast<uchar>((d.disparity.row -
    //        minVal)*255/(maxVal-minVal));
    auto colColor = static_cast<uchar>((d.disparity.col - minVal) * 255 /
                                       (maxVal - minVal));
    cv::circle(coloredMap, cv::Point(d.leftPos.col, d.leftPos.row), 2,
               cv::Scalar(colColor, 0, 0), -1);
  }
  displayInNewWindow("disp", coloredMap);

  return 0;
}