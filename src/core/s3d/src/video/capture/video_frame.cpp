//
// Created by bedh2102 on 06/04/17.
//

#include "s3d/video/video_frame.h"

// static
size_t VideoFrame::AllocationSize(VideoPixelFormat format, const Size& size) {
  return NumBytesPerPixel(format) * size.getWidth() * size.getHeight();
}

// static
size_t VideoFrame::NumBytesPerPixel(VideoPixelFormat format) {
  switch (format) {
    case VideoPixelFormat::ARGB:
      return 4;
    case VideoPixelFormat::UYVY:
      return 2;
    case VideoPixelFormat::RGB:
      return 3;
    case VideoPixelFormat::UNKNOWN:
      return 0;
  }
}