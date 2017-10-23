#include "s3d/rectification/rectification_stan.h"

namespace s3d {

// static
Eigen::Matrix3f RectificationStan::leftImageMatrix(const StanAlignment& alignment) {
  auto&& a = alignment;

  Eigen::Matrix3f H;
  H << 1.0f, a.ch_y, 0.0f,  //
      -a.ch_y, 1.0f, 0.0f,  //
      -a.ch_z_f, 0.0f, 1.0f;

  return H;
}

// static
Eigen::Matrix3f RectificationStan::rightImageMatrix(const StanAlignment& alignment) {
  auto&& a = alignment;

  Eigen::Matrix3f Hp;
  Hp << 1 - a.a_f, a.a_z + a.ch_y, 0,         //
      -(a.a_z + a.ch_y), 1 - a.a_f, a.f_a_x,  //
      a.a_y_f - a.ch_z_f, -a.a_x_f, 1;

  return Hp;
}

}  // namespace s3d
