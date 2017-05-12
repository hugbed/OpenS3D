#ifndef S3D_IMAGE_IMAGE_H
#define S3D_IMAGE_IMAGE_H

#include "s3d/image/image_container.h"
#include "s3d/image/pixel_types.h"

#include <cstdint>  // uint8_t

namespace s3d {
template class Image<uint8_t>;
template class Image<RGBColor>;
}

#endif  // S3D_IMAGE_IMAGE_H
