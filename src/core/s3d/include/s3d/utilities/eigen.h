//
// Created by jon on 09/05/17.
//

#ifndef S3D_UTILITIES_EIGEN_H
#define S3D_UTILITIES_EIGEN_H

#include <Eigen/Dense>

namespace s3d {

// todo: to be tested

template <typename _Matrix_Type_>
_Matrix_Type_ pseudoinverse(const _Matrix_Type_& a,
                            double epsilon = std::numeric_limits<double>::epsilon()) {
  Eigen::JacobiSVD<_Matrix_Type_> svd(a, Eigen::ComputeThinU | Eigen::ComputeThinV);
  double tolerance = epsilon * std::max(a.cols(), a.rows()) * svd.singularValues().array().abs()(0);
  return svd.matrixV() *
         (svd.singularValues().array().abs() > tolerance)
             .select(svd.singularValues().array().inverse(), 0)
             .matrix()
             .asDiagonal() *
         svd.matrixU().adjoint();
}

}  // namespace s3d

#endif  // S3D_UTILITIES_EIGEN_H
