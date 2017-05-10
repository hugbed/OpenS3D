//
// Created by jon on 09/05/17.
//

#ifndef S3D_UTILITIES_EIGEN_H
#define S3D_UTILITIES_EIGEN_H

#include <Eigen/Dense>

namespace s3d {

// todo: to be tested

template <class MatT>
// Eigen::Matrix<typename MatT::Scalar, MatT::ColsAtCompileTime, MatT::RowsAtCompileTime>
auto pseudoinverse(
    const MatT& mat,
    typename MatT::Scalar tolerance = typename MatT::Scalar{1e-4})  // choose appropriately
{
  typedef typename MatT::Scalar Scalar;
  auto svd = mat.jacobiSvd(Eigen::ComputeFullU | Eigen::ComputeFullV);
  const auto& singularValues = svd.singularValues();
  Eigen::Matrix<Scalar, MatT::ColsAtCompileTime, MatT::RowsAtCompileTime> singularValuesInv(
      mat.cols(), mat.rows());
  singularValuesInv.setZero();
  for (unsigned int i = 0; i < singularValues.size(); ++i) {
    if (singularValues(i) > tolerance) {
      singularValuesInv(i, i) = Scalar{1} / singularValues(i);
    } else {
      singularValuesInv(i, i) = Scalar{0};
    }
  }
  return svd.matrixV() * singularValuesInv * svd.matrixU().adjoint();
}

template <typename _Matrix_Type_>
_Matrix_Type_ pinv(const _Matrix_Type_& a,
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
