// Copyright 2013 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#include "s3d/video/capture/video_capture_types.h"
#include "s3d/video/video_frame.h"

#include <algorithm>

VideoCaptureFormat::VideoCaptureFormat()
    : frameRate{0.0f}, pixelFormat{VideoPixelFormat::UNKNOWN}, stereo3D{false} {}

VideoCaptureFormat::VideoCaptureFormat(Size frameSize,
                                       float frameRate,
                                       VideoPixelFormat pixelFormat)
    : frameSize{frameSize}, frameRate{frameRate}, pixelFormat{pixelFormat}, stereo3D{false} {}

VideoCaptureFormat::VideoCaptureFormat(Size frameSize,
                                       float frameRate,
                                       VideoPixelFormat pixelFormat,
                                       bool stereo3D)
    : frameSize{frameSize}, frameRate{frameRate}, pixelFormat{pixelFormat}, stereo3D{stereo3D} {}

size_t VideoCaptureFormat::ImageAllocationSize() const {
  auto sizeInBytes = VideoFrame::AllocationSize(pixelFormat, frameSize);
  return stereo3D ? 2 * sizeInBytes : sizeInBytes;
}
