// Copyright 2012 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_TYPES_H
#define S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_TYPES_H

#include "s3d/geometry/size.h"
#include "s3d/video/video_types.h"

#include <cstddef>
#include <tuple>

// todo(hugbed): put in s3d namespace

struct VideoCaptureFormat {
  constexpr VideoCaptureFormat()
      : frameRate{-1.0f}, pixelFormat{VideoPixelFormat::UNKNOWN}, stereo3D{false} {}
  constexpr VideoCaptureFormat(Size frameSize, float frameRate, VideoPixelFormat pixelFormat)
      : frameSize{frameSize}, frameRate{frameRate}, pixelFormat{pixelFormat}, stereo3D{false} {}
  constexpr VideoCaptureFormat(Size frameSize,
                               float frameRate,
                               VideoPixelFormat pixelFormat,
                               bool stereo3D)
      : frameSize{frameSize}, frameRate{frameRate}, pixelFormat{pixelFormat}, stereo3D{stereo3D} {}

  constexpr VideoCaptureFormat(const VideoCaptureFormat& other)
      : frameSize{other.frameSize},
        frameRate{other.frameRate},
        pixelFormat{other.pixelFormat},
        stereo3D{other.stereo3D} {}

  size_t ImageAllocationSize() const;

  constexpr bool operator==(const VideoCaptureFormat& other) const {
    return std::make_tuple(frameSize, frameRate, pixelFormat, stereo3D) ==
           std::make_tuple(other.frameSize, other.frameRate, other.pixelFormat, other.stereo3D);
  }

  Size frameSize{};
  float frameRate;
  VideoPixelFormat pixelFormat;
  bool stereo3D;
};

#endif  // S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_TYPES_H
