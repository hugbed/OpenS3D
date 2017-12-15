// Copyright 2012 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#include "s3d/video/video_frame.h"

namespace s3d {

// static
size_t VideoFrame::AllocationSize(VideoPixelFormat format, const Size& size) {
  return NumBytesPerPixel(format) * size.getWidth() * size.getHeight();
}

// static
size_t VideoFrame::NumBytesPerPixel(VideoPixelFormat format) {
  switch (format) {
    case VideoPixelFormat::ARGB:
    case VideoPixelFormat::BGRA:
      return 4;
    case VideoPixelFormat::UYVY:
      return 2;
    case VideoPixelFormat::RGB:
    case VideoPixelFormat::BGR:
      return 3;
  }
  return 0;
}

}  // namespace s3d
