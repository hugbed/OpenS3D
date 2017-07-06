// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S3D_VIDEO_VIDEO_FRAME_H
#define S3D_VIDEO_VIDEO_FRAME_H

#include "s3d/geometry/size.h"
#include "s3d/video/video_types.h"

#include <cstddef>
#include <vector>
#include <chrono>

// todo(hugbed): put in s3d namespace

class VideoFrame {
 public:
  VideoFrame(std::vector<uint8_t> data,
             std::chrono::microseconds timestamp = std::chrono::microseconds(0))
      : data_(std::move(data)), timestamp_(timestamp) {}

  static size_t AllocationSize(VideoPixelFormat format, const Size& size);

  std::vector<uint8_t> data_;
  std::chrono::microseconds timestamp_;

 private:
  static size_t NumBytesPerPixel(VideoPixelFormat format);
};

#endif  // S3D_VIDEO_VIDEO_FRAME_H
