//
// Created by bedh2102 on 06/04/17.
//

#ifndef PROJECT_VIDEOFRAME_H
#define PROJECT_VIDEOFRAME_H

#include "s3d/video/video_types.h"
#include "s3d/geometry/Size.h"

#include <cstddef>

class VideoFrame {
public:
  static size_t AllocationSize(VideoPixelFormat format, const Size& size);
};

#endif //PROJECT_VIDEOFRAME_H
