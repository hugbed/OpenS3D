// Copyright 2013 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#include "s3d/video/capture/video_capture_types.h"
#include "s3d/video/video_frame.h"

namespace s3d {

size_t VideoCaptureFormat::ImageAllocationSize() const {
  return VideoFrame::AllocationSize(pixelFormat, frameSize);
}

}  // namespace s3d
