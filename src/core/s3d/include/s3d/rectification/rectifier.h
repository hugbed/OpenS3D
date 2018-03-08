#ifndef S3D_RECTIFIER_H
#define S3D_RECTIFIER_H

#include "s3d/utilities/rule_of_five.h"

#include "s3d/image/image_container.h"

#include "s3d/utilities/eigen.h"

namespace s3d {

class Rectifier : rule_of_five_interface<Rectifier> {
  // todo: analyse performance cost for always allocating a new image
  // to return instead of always passing an already allocated image as a pointer in parameter
  virtual Image<uint8_t> rectify(const Image<uint8_t>& image, Eigen::Matrix3d H) = 0;
};
}  // namespace s3d

#endif  // S3D_RECTIFIER_H
