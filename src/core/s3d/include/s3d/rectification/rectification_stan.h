#ifndef S3D_MULTIVIEW_STAN_RECTIFICATION_H
#define S3D_MULTIVIEW_STAN_RECTIFICATION_H

#include "s3d/multiview/stan_fundamental_matrix_solver.h"
#include <s3d/geometry/size.h>

#include <Eigen/Dense>

namespace s3d {

/**
 * Retrieve rectification matrices from stereo rig alignment.
 * Correction of rig alignment displacements in images can be
 * done using these matrices with a Rectifier.
 */
class RectificationStan {
 public:
  /**
   * Rectification matrix from stereo rig alignment for left image.
   *
   * @param alignment Stereo rig alignment found with DisparityAnalyzerSTAN
   * @param imageSize Image size to center the rectification
   * @return rectification matrix to apply to the left image
   */
  static Eigen::Matrix3f leftImageMatrix(const StanAlignment& alignment);

  /**
   * Rectification matrix from stereo rig alignment for right image.
   *
   * @param alignment Stereo rig alignment found with DisparityAnalyzerSTAN
   * @param imageSize Image size to center the rectification
   * @return rectification matrix to apply to the right image
   */
  static Eigen::Matrix3f rightImageMatrix(const StanAlignment& alignment);

  /**
   * Use when alignment was found with image points centered around image center
   */
  static Eigen::Matrix3f centeredLeftImageMatrix(const StanAlignment& alignment, const Size& imageSize);

  /**
   * Use when alignment was found with image points centered around image center
   */
  static Eigen::Matrix3f centeredRightImageMatrix(const StanAlignment& alignment, const Size& imageSize);

  /**
   * Transform rectification about image center
   *
   * @param rectification matrix about top left corner
   * @param imageSize
   * @return rectification matrix about image center
   */
  static Eigen::Matrix3f centerRectification(Eigen::Matrix3f H, const Size& imageSize);
};

}  // namespace s3d

#endif  // S3D_MULTIVIEW_STAN_RECTIFICATION_H
