//
// Created by jon on 06/03/17.
//

#ifndef OPENCVTESTS_DISPARITYALGORITHMBM_H
#define OPENCVTESTS_DISPARITYALGORITHMBM_H

#include "s3d/disparity/disparity_algorithm_bm.h"
#include "s3d/disparity/disparities_dense.h"
#include "s3d/utilities/time.h"

#include "opencv2/cudastereo.hpp"
#include "opencv2/cudafeatures2d.hpp"
#include "opencv2/cudaarithm.hpp"

#include <iostream>
#include <memory>

namespace s3d {

std::unique_ptr<Disparities> DisparityAlgorithmBM::ComputeDisparities(
    Image leftImg,
    Image rightImg) {
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

  auto disparities =
      std::unique_ptr<Disparities>(std::make_unique<DisparitiesDense>(
          disp, DisparityPoint{{minPos.y, minPos.x}, {0, int(minVal)}},
          DisparityPoint{{maxPos.y, maxPos.x}, {0, int(maxVal)}}));

  return disparities;
}
}

#endif  // OPENCVTESTS_DISPARITYALGORITHMBM_H
