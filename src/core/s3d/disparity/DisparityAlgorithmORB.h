//
// Created by jon on 06/03/17.
//

#ifndef OPENCVTESTS_DISPARITYALGORITHMORB_H
#define OPENCVTESTS_DISPARITYALGORITHMORB_H

#include "DisparityAlgorithm.h"
#include "../hb_utils.h"

#include "opencv2/cudastereo.hpp"
#include "opencv2/cudafeatures2d.hpp"
#include "opencv2/cudaarithm.hpp"

#include <iostream>
#include <memory>

struct ImageSize
{
    size_t rows;
    size_t cols;
};

class DisparitiesSparse : public Disparities
{
public:
    DisparitiesSparse(const std::vector<DisparityPoint> &disparities, ImageSize imageSize)
            : disparities_(disparities)
            , disparityMap_(cv::Mat::zeros(static_cast<int>(imageSize.rows), static_cast<int>(imageSize.cols), CV_8U))
    {
    }

    virtual const std::vector<DisparityPoint> &getDisparities() override // const & ??
    {
        return disparities_;
    }

    virtual Image getDisparityMap() override
    {
        auto minVal = min().disparity.col;
        auto maxVal = max().disparity.col;

        // this seems so inefficient
        for (auto d_it = std::begin(disparities_); d_it != std::end(disparities_); ++d_it) {

            auto i = (*d_it).leftPos.row;
            auto j = (*d_it).leftPos.col;
            auto d = (*d_it).disparity.col;

            disparityMap_.mat.at<uchar>(i, j) = static_cast<uchar>((d - minVal)*255/(maxVal-minVal));
        }
        return disparityMap_;
    }

    virtual DisparityPoint max() override
    {
        return *(std::max(std::begin(disparities_), std::end(disparities_)));
    }

    virtual DisparityPoint min() override
    {
        return *(std::min(std::begin(disparities_), std::end(disparities_)));
    }

private:
    std::vector<DisparityPoint> disparities_;
    Image disparityMap_;
};

class DisparityAlgorithmORB : public DisparityAlgorithm
{
public:
    std::unique_ptr<Disparities> ComputeDisparities(Image leftImg, Image rightImg) override
    {
        auto left = leftImg.mat;
        auto right = rightImg.mat;

        cv::cuda::GpuMat d_left, d_right;
        d_left.upload(left);
        d_right.upload(right);

        std::vector<cv::KeyPoint> keypoints1, keypoints2;
        std::vector<cv::DMatch> matches;

        auto orb = cv::cuda::ORB::create();//200, 1.2f, 8, 31, 0, 2, 0, 31, 20, true);

        cv::cuda::GpuMat d_keypoints1, d_descriptors1, d_descriptors2;
        orb->detectAndComputeAsync(d_left, cv::cuda::GpuMat(), d_keypoints1, d_descriptors1);
        orb->convert(d_keypoints1, keypoints1);
        orb->detectAndCompute(d_right, cv::cuda::GpuMat(), keypoints2, d_descriptors2);

        std::vector<std::vector<cv::DMatch>> knn_matches;
        auto matcher = cv::cuda::DescriptorMatcher::createBFMatcher(cv::NORM_HAMMING);
        matcher->knnMatch(d_descriptors2, d_descriptors1, knn_matches, 2);

        std::vector<DisparityPoint> disparityPoints;

        for(auto it = std::begin(knn_matches); it != std::end(knn_matches); ++it) {
            if(it->size() > 1 && (*it)[0].distance/(*it)[1].distance < 0.8) {
                matches.push_back((*it)[0]);

                auto img1Idx = (*it)[0].queryIdx;
                auto img2Idx = (*it)[0].trainIdx;
                auto pt1 = keypoints1[img1Idx].pt;
                auto pt2 = keypoints2[img2Idx].pt;
                auto d = pt2 - pt1;

                disparityPoints.emplace_back(DisparityPoint({int(pt1.y), int(pt1.x)}, {int(d.y), int(d.x)}));
            }
        }

//        cv::Mat imgRes;
//        cv::drawMatches(right, keypoints2, left, keypoints1, matches, imgRes);
//        cv::imshow("imgRes", imgRes);

        auto disparities = std::unique_ptr<Disparities>(
                std::make_unique<DisparitiesSparse>(disparityPoints, ImageSize{size_t(left.rows), size_t(left.cols)}));

        return disparities;
    }
};


#endif //OPENCVTESTS_DISPARITYALGORITHMORB_H
