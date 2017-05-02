// Copyright 2012 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_TYPES_H
#define S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_TYPES_H

#include "s3d/geometry/size.h"
#include "s3d/video/video_types.h"

#include <cstddef>

// todo(hugbed): put in s3d namespace

struct VideoCaptureFormat {
  class Unsupported;

  constexpr VideoCaptureFormat()
      : frameRate{-1.0f}, pixelFormat{VideoPixelFormat::UNKNOWN}, stereo3D{false} {}
  constexpr VideoCaptureFormat(Size frameSize, float frameRate, VideoPixelFormat pixelFormat)
      : frameSize{frameSize}, frameRate{frameRate}, pixelFormat{pixelFormat}, stereo3D{false} {}
  constexpr VideoCaptureFormat(Size frameSize,
                               float frameRate,
                               VideoPixelFormat pixelFormat,
                               bool stereo3D)
      : frameSize{frameSize}, frameRate{frameRate}, pixelFormat{pixelFormat}, stereo3D{stereo3D} {}
  size_t ImageAllocationSize() const;

  constexpr bool operator==(const VideoCaptureFormat& other) const {
    return frameSize == other.frameSize && frameRate == other.frameRate &&
           pixelFormat == other.pixelFormat && stereo3D == other.stereo3D;
  }

  Size frameSize;
  float frameRate;
  VideoPixelFormat pixelFormat;
  bool stereo3D;
};

#endif  // S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_TYPES_H
