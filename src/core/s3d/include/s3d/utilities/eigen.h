#ifndef S3D_UTILITIES_EIGEN_H
#define S3D_UTILITIES_EIGEN_H

#include <Eigen/Dense>
#include <Eigen/SVD>

#include<Eigen/StdVector>

// to prevent alignment issues with std::vector
#ifndef EIGEN_SPECIALIZED
#define EIGEN_SPECIALIZED
  EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Vector2d);
  EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Vector3d);
#endif // EIGEN_SPECIALIZED

namespace s3d {

template <class MatT>
Eigen::Matrix<typename MatT::Scalar, MatT::ColsAtCompileTime, MatT::RowsAtCompileTime>
pseudoinverse(
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

}  // namespace s3d

#endif  // S3D_UTILITIES_EIGEN_H
