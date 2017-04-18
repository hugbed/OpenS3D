// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_TYPES_H
#define S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_TYPES_H

#include "s3d/geometry/size.h"
#include "s3d/video/video_types.h"

#include <cstddef>

// todo(hugbed): put in s3d namespace

struct VideoCaptureFormat {
  VideoCaptureFormat();
  VideoCaptureFormat(Size frameSize, float frameRate, VideoPixelFormat pixelFormat);
  VideoCaptureFormat(Size frameSize, float frameRate, VideoPixelFormat pixelFormat, bool stereo3D);
  size_t ImageAllocationSize() const;

  Size frameSize;
  float frameRate;
  VideoPixelFormat pixelFormat;
  bool stereo3D;
};

#endif  // S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_TYPES_H
