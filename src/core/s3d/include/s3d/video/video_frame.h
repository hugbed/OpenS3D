//
// Created by bedh2102 on 06/04/17.
//

#ifndef S3D_VIDEO_VIDEO_FRAME_H
#define S3D_VIDEO_VIDEO_FRAME_H

#include "s3d/geometry/size.h"
#include "s3d/video/video_types.h"

#include <cstddef>

// todo(hugbed): put in s3d namespace

class VideoFrame {
 public:
  static size_t AllocationSize(VideoPixelFormat format, const Size& size);

 private:
  static size_t NumBytesPerPixel(VideoPixelFormat format);
};

#endif  // S3D_VIDEO_VIDEO_FRAME_H
