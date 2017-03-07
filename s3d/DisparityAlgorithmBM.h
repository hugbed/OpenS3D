//
// Created by jon on 06/03/17.
//

#ifndef OPENCVTESTS_DISPARITYALGORITHMBM_H
#define OPENCVTESTS_DISPARITYALGORITHMBM_H

#include "DisparityAlgorithm.h"
#include "../hb_utils.h"

#include "opencv2/cudastereo.hpp"
#include "opencv2/cudafeatures2d.hpp"
#include "opencv2/cudaarithm.hpp"

#include <iostream>
#include <memory>

class DisparitiesDense : public Disparities
{
public:
    DisparitiesDense(Image disparityMap,
                     DisparityPoint minDisparity,
                     DisparityPoint maxDisparity)
        : disparityMap_(disparityMap)
        , min_(minDisparity)
        , max_(maxDisparity)
    {
    }

    virtual const std::vector<DisparityPoint> &getDisparities() override
    {
        if (disparities_.size() > 0) return disparities_;
        for( int i = 0; i < disparityMap_.mat.rows; ++i) {
            for (int j = 0; j < disparityMap_.mat.cols; ++j) {
                auto dx = disparityMap_.mat.at<uchar>(i, j);
                disparities_.emplace_back(DisparityPoint({i, j}, {0, dx}));
            }
        }
        return disparities_;
    }

    virtual Image getDisparityMap() override
    {
        return disparityMap_;
    }

    virtual DisparityPoint max() override
    {
        return max_;
    }

    virtual DisparityPoint min() override
    {
        return min_;
    }

private:
    Image disparityMap_;
    std::vector<DisparityPoint> disparities_;
    DisparityPoint min_;
    DisparityPoint max_;
};

class DisparityAlgorithmBM : public DisparityAlgorithm
{
public:
    std::unique_ptr<Disparities> ComputeDisparities(Image leftImg, Image rightImg) override
    {
        auto left = leftImg.mat;
        auto right = rightImg.mat;

        // load into gpu
        cv::cuda::GpuMat d_left, d_right;
        d_left.upload(left);
        d_right.upload(right);

        // Prepare disparity map of specified type
        cv::Mat disp(left.size(), CV_8U);
        cv::cuda::GpuMat d_disp(left.size(), CV_8U);

        // compute disparity map
        auto bm = cv::cuda::createStereoBM();
        bm->compute(d_left, d_right, d_disp);

        // filter disparity map

        // retrieve max and min values
        cv::Point minPos, maxPos;
        double minVal, maxVal;
        cv::cuda::minMaxLoc(d_disp, &minVal, &maxVal, &minPos, &maxPos);

        d_disp.download(disp);

        auto disparities = std::unique_ptr<Disparities>(
                std::make_unique<DisparitiesDense>(disp,
                                                DisparityPoint{{minPos.y, minPos.x}, {0, int(minVal)}},
                                                DisparityPoint{{maxPos.y, maxPos.x}, {0, int(maxVal)}}));

        return disparities;
    }
};


#endif //OPENCVTESTS_DISPARITYALGORITHMBM_H
