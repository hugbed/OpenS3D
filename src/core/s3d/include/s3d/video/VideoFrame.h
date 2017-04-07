//
// Created by bedh2102 on 06/04/17.
//

#ifndef PROJECT_VIDEOFRAME_H
#define PROJECT_VIDEOFRAME_H

#include "s3d/video/video_types.h"

#include <cstddef>

class Size;

class VideoFrame {
public:
  static size_t AllocationSize(VideoPixelFormat format, const Size& size);
private:
  static size_t NumPlanes(VideoPixelFormat format);
  static Size PlaneSize(VideoPixelFormat format, size_t plane, const Size& size);
  static bool RequiresEvenSizeAllocation(VideoPixelFormat format);
  static Size SampleSize(VideoPixelFormat format, size_t plane);
  static int BytesPerElement(VideoPixelFormat format, size_t plane);
};

#endif //PROJECT_VIDEOFRAME_H
