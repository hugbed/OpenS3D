// Copyright 2014 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#include "s3d/video/capture/file_video_capture_device_factory.h"

#include "s3d/video/capture/file_video_capture_device.h"

namespace s3d {

std::unique_ptr<VideoCaptureDevice> FileVideoCaptureDeviceFactory::CreateDevice(
    const VideoCaptureDeviceDescriptor& deviceDescriptor) const {
  return std::unique_ptr<VideoCaptureDevice>(new FileVideoCaptureDevice(deviceDescriptor.name));
}

}  // namespace s3d
