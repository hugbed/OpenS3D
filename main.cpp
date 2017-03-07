#include <iostream>

#include "hb_utils.h"
#include "opencv2/cudastereo.hpp"
#include "opencv2/cudafeatures2d.hpp"
#include <array>

#include "s3d/DisparityAlgorithmBM.h"
#include "s3d/DisparityAlgorithmORB.h"

using namespace cv;

int main() {
    std::cout << "Hello, World!" << std::endl;

    auto left = imread("/home/jon/ClionProjects/OpenCVTests/left.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    auto right = imread("/home/jon/ClionProjects/OpenCVTests/right.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    assert(left.data);
    assert(right.data);

    // debugging
    cv::Mat combined(left.rows, left.cols*2, left.type());
    cv::hconcat(left, right, combined);
    hb::displayInNewWindow("before", combined);

    // Compute disparities
    auto disparityAlgo = std::unique_ptr<DisparityAlgorithm>(
            std::make_unique<DisparityAlgorithmORB>());
    auto disparities = disparityAlgo->ComputeDisparities({left}, {right});

    // Display disparity map
    Mat cm_disp;
    auto disp = disparities->getDisparityMap();
    applyColorMap(disp.mat, cm_disp, COLORMAP_OCEAN);
    hb::displayInNewWindow("dispMap", cm_disp);

    // Display disparity points
    cv::Mat coloredMap(left.size(), CV_8UC3, cv::Scalar(0, 0, 0));
    cv::cvtColor(left, coloredMap, CV_GRAY2BGR);

    auto minVal = disparities->min().disparity.col;
    auto maxVal = disparities->max().disparity.col;

    auto disparityPoints = disparities->getDisparities();
    for (auto& d : disparityPoints)
    {
//        auto rowColor = static_cast<uchar>((d.disparity.row - minVal)*255/(maxVal-minVal));
        auto colColor = static_cast<uchar>((d.disparity.col - minVal)*255/(maxVal-minVal));
        cv::circle(coloredMap, cv::Point(d.leftPos.col, d.leftPos.row), 2, cv::Scalar(colColor, 0, 0), -1);
    }
    hb::displayInNewWindow("disp", coloredMap);

    return 0;
}