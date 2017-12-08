//
// Created by jon on 12/8/17.
//

#ifndef UTILITIES_EIGEN_H
#define UTILITIES_EIGEN_H

#include <QtGui/QMatrix3x3>

#include <Eigen/Dense>

template <class T, int M, int N>
QGenericMatrix<M, N, T> eigenToQMatrix(const Eigen::Matrix<T, M, N>& matrix) {
  Eigen::Matrix<T, M, N, Eigen::RowMajor> matrixCopy = matrix.template cast<T>();
  QGenericMatrix<M, N, T> qMatrix(matrixCopy.data());
  return qMatrix;
}

#endif //UTILITIES_EIGEN_H
