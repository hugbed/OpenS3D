#ifndef S3D_MULTIVIEW_STAN_RECTIFICATION_H
#define S3D_MULTIVIEW_STAN_RECTIFICATION_H

#include "s3d/rectification/rectifier.h"

#include "s3d/geometry/size.h"
#include "s3d/multiview/stan_fundamental_matrix_solver.h"

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
  static Eigen::Matrix3f leftImageMatrix(const StanAlignment& alignment) {
    auto&& a = alignment;

    Eigen::Matrix3f H;
    H << 1.0f, a.ch_y, 0.0f,  //
        -a.ch_y, 1.0f, 0.0f,  //
        -a.ch_z_f, 0.0f, 1.0f;

    return H;
  }

  /**
   * Rectification matrix from stereo rig alignment for right image.
   *
   * @param alignment Stereo rig alignment found with DisparityAnalyzerSTAN
   * @param imageSize Image size to center the rectification
   * @return rectification matrix to apply to the right image
   */
  static Eigen::Matrix3f rightImageMatrix(const StanAlignment& alignment) {
    auto&& a = alignment;

    Eigen::Matrix3f Hp;
    Hp << 1 - a.a_f, a.a_z + a.ch_y, 0,         //
        -(a.a_z + a.ch_y), 1 - a.a_f, a.f_a_x,  //
        a.a_y_f - a.ch_z_f, -a.a_x_f, 1;

    return Hp;

    // return HOffT * Hp * HOff;
  }
};

}  // namespace s3d

#endif  // S3D_MULTIVIEW_STAN_RECTIFICATION_H
