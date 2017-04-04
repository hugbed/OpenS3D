//
// Created by jon on 06/03/17.
//

#ifndef S3D_DISPARITY_DISPARITYALGORITHM_H
#define S3D_DISPARITY_DISPARITYALGORITHM_H

#include <vector>
#include <memory>
#include <cv.h>

struct Image {
    Image(cv::Mat imgMat)
            : mat(imgMat) {}
    cv::Mat mat;
};

struct PixelPos{
    int row; int col;
};

struct DisparityPoint
{
    DisparityPoint(PixelPos leftLoc, PixelPos disparityValue)
        : leftPos(leftLoc)
        , disparity(disparityValue) {}

    PixelPos leftPos;
    PixelPos disparity;

    bool operator<(const DisparityPoint &d2)
    {
        return disparity.col < d2.disparity.col;
    }

    bool operator>(const DisparityPoint &d2)
    {
        return disparity.col > d2.disparity.col;
    }
};

class Disparities {
public:
    virtual const std::vector<DisparityPoint> &getDisparities() = 0;
    virtual Image getDisparityMap() = 0;
    virtual DisparityPoint max() = 0;
    virtual DisparityPoint min() = 0;
};

class DisparityAlgorithm
{
public:
    virtual std::unique_ptr<Disparities> ComputeDisparities(Image left, Image right) = 0;
};

#endif //S3D_DISPARITY_DISPARITYALGORITHM_H
