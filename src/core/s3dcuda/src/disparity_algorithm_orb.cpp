//
// Created by jon on 04/04/17.
//

#include <s3d/disparity/disparities_sparse.h>
#include "s3dcuda/disparity/disparity_algorithm_orb.h"

#include "opencv2/cudaarithm.hpp"
#include "opencv2/cudafeatures2d.hpp"
#include "opencv2/cudastereo.hpp"

namespace s3d {
namespace cuda {

std::unique_ptr<Disparities> DisparityAlgorithmORB::ComputeDisparities(Image leftImg,
                                                                       Image rightImg) {
  auto left = leftImg.mat;
  auto right = rightImg.mat;

  cv::cuda::GpuMat d_left, d_right;
  d_left.upload(left);
  d_right.upload(right);

  std::vector<cv::KeyPoint> keypoints1, keypoints2;
  std::vector<cv::DMatch> matches;

  auto orb = cv::cuda::ORB::create();  // 200, 1.2f, 8, 31, 0, 2, 0, 31, 20, true);

  cv::cuda::GpuMat d_keypoints1, d_descriptors1, d_descriptors2;
  orb->detectAndComputeAsync(d_left, cv::cuda::GpuMat(), d_keypoints1, d_descriptors1);
  orb->convert(d_keypoints1, keypoints1);
  orb->detectAndCompute(d_right, cv::cuda::GpuMat(), keypoints2, d_descriptors2);

  std::vector<std::vector<cv::DMatch>> knn_matches;
  auto matcher = cv::cuda::DescriptorMatcher::createBFMatcher(cv::NORM_HAMMING);
  matcher->knnMatch(d_descriptors2, d_descriptors1, knn_matches, 2);

  std::vector<DisparityPoint> disparityPoints;

  for (auto it = std::begin(knn_matches); it != std::end(knn_matches); ++it) {
    if (it->size() > 1 && (*it)[0].distance / (*it)[1].distance < 0.8) {
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
  //        cv::drawMatches(right, keypoints2, left, keypoints1, matches,
  //        imgRes);
  //        cv::imshow("imgRes", imgRes);

  auto disparities = std::unique_ptr<Disparities>(std::make_unique<DisparitiesSparse>(
      disparityPoints, ImageSize{size_t(left.rows), size_t(left.cols)}));

  return disparities;
}
}  // namespace cuda
}  // namespace s3d