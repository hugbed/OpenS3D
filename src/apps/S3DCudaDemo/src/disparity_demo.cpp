#include "s3dcuda/disparity/disparity_algorithm_bm.h"
#include "s3dcuda/disparity/disparity_algorithm_orb.h"

#include "Eigen/Dense"

#include <opencv2/opencv.hpp>
#include <opencv2/cudafeatures2d.hpp>
#include <opencv2/cudastereo.hpp>
#include <opencv/cxeigen.hpp>

#include "gsl/gsl"

using s3d::DisparityAlgorithm;
using s3d::cuda::DisparityAlgorithmORB;
using s3d::Image;

void displayInNewWindow(const std::string& name, cv::InputArray src) {
  cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
  cv::imshow(name, src);
  cv::waitKey(0);
}

class BadNumberOfArgumentsException {};
class FileNotFoundException {};

int main(int argc, char* argv[]) {
  auto input_args = gsl::span<char*>(argv, argc);
  if (input_args.size() != 3) {
    throw BadNumberOfArgumentsException{};
  }

  auto path_left = std::string(input_args[1]);
  auto path_right = std::string(input_args[2]);

  auto left = cv::imread(path_left, CV_LOAD_IMAGE_GRAYSCALE);
  auto right = cv::imread(path_right, CV_LOAD_IMAGE_GRAYSCALE);
  if (left.data == nullptr || right.data == nullptr) {
    throw FileNotFoundException{};
  }

  //  cv::resize(left, left, cv::Size(left.rows / 2, left.cols / 2));
  //  cv::resize(right, right, cv::Size(left.rows / 2, left.cols / 2));

  // debugging
  cv::Mat combined(left.rows, left.cols * 2, left.type());
  cv::hconcat(left, right, combined);
  displayInNewWindow("before", combined);

  Image<uint8_t> leftImg(Size(left.rows, left.cols));
  Image<uint8_t> rightImg(Size(right.rows, right.cols));
  cv::cv2eigen(left, leftImg);
  cv::cv2eigen(right, rightImg);

  // Compute disparities
  auto disparityAlgo =
      std::unique_ptr<DisparityAlgorithm>(std::make_unique<DisparityAlgorithmORB>());
  auto disparities = disparityAlgo->ComputeDisparities(leftImg, rightImg);

  // Display disparity map
  cv::Mat cm_disp;
  auto disp = disparities->getDisparityMap();
  cv::eigen2cv(disp, cm_disp);
  applyColorMap(cm_disp, cm_disp, cv::COLORMAP_OCEAN);
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
    auto colColor = static_cast<uchar>((d.disparity.col - minVal) * 255 / (maxVal - minVal));
    cv::circle(coloredMap, cv::Point(d.leftPos.col, d.leftPos.row), 2, cv::Scalar(colColor, 0, 0),
               -1);
  }
  displayInNewWindow("disp", coloredMap);

  return 0;
}
