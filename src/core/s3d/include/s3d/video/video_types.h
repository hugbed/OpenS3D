// Copyright 2012 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S3D_VIDEO_VIDEO_TYPES_H
#define S3D_VIDEO_VIDEO_TYPES_H

namespace s3d {

enum class VideoPixelFormat { UNKNOWN = 0, UYVY, ARGB, BGRA, BGR, RGB };

enum class Stereo3DFormat {
  Separate,
  SideBySide,
  SideBySideHalf,
  AboveBelow,
  AboveBelowHalf
};  // include half resolution

}  // namespace s3d

#endif  // S3D_VIDEO_VIDEO_TYPES_H
