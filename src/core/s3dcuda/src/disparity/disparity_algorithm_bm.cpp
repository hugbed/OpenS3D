#include "s3dcuda/disparity/disparity_algorithm_bm.h"
#include "s3d/disparity/disparities_dense.h"
#include "s3d/utilities/time.h"

#include "opencv2/cudaarithm.hpp"
#include "opencv2/cudafeatures2d.hpp"
#include "opencv2/cudastereo.hpp"
#include "opencv/cxeigen.hpp"

#include <iostream>
#include <memory>

// todo(hugbed): move this to s3dcuda

namespace s3d {
namespace cuda {

std::unique_ptr<Disparities> DisparityAlgorithmBM::ComputeDisparities(Image<uint8_t> leftImg,
                                                                      Image<uint8_t> rightImg) {
  cv::Mat left;
  cv::Mat right;
  cv::eigen2cv(leftImg.getMatrix(), left);
  cv::eigen2cv(rightImg.getMatrix(), right);

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

  Image<uint8_t> dispImg(Size(disp.rows, disp.cols));
  cv::cv2eigen(disp, dispImg);

  auto disparities = std::unique_ptr<Disparities>(std::make_unique<DisparitiesDense>(
      dispImg, DisparityPoint{PixelPos{minPos.y, minPos.x}, PixelPos{0, int(minVal)}},
      DisparityPoint{PixelPos{maxPos.y, maxPos.x}, PixelPos{0, int(maxVal)}}));

  return disparities;
}

}  // namespace cuda
}  // namespace s3d
