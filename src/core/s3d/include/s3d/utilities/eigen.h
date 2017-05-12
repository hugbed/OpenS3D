//
// Created by jon on 09/05/17.
//

#ifndef S3D_UTILITIES_EIGEN_H
#define S3D_UTILITIES_EIGEN_H

#include <Eigen/Dense>

namespace s3d {

template <class MatrixType>
struct is_eigen_dynamic {
  enum {
    value = MatrixType::ColsAtCompileTime == Eigen::Dynamic &&
            MatrixType::RowsAtCompileTime == Eigen::Dynamic
  };
};

// for dynamic matrices only
// for square matrices: call inverse()
template <class MatrixType,
          class DynamicSizeMatrixType =
              typename std::enable_if<is_eigen_dynamic<MatrixType>::value, MatrixType>::type>
DynamicSizeMatrixType pseudoinverse(const MatrixType& a,
                                    double epsilon = std::numeric_limits<double>::epsilon()) {
  Eigen::JacobiSVD<MatrixType> svd(a, Eigen::ComputeThinU | Eigen::ComputeThinV);
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
