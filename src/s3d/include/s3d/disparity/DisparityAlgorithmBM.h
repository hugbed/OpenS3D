//
// Created by jon on 06/03/17.
//

#ifndef S3D_DISPARITY_DISPARITYALGORITHMBM_H
#define S3D_DISPARITY_DISPARITYALGORITHMBM_H

#include "DisparityAlgorithm.h"

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
    std::unique_ptr<Disparities> ComputeDisparities(Image leftImg, Image rightImg) override;
};


#endif //S3D_DISPARITY_DISPARITYALGORITHMBM_H
