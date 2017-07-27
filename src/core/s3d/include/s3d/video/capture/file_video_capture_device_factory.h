// Copyright 2014 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_FACTORY_H
#define S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_FACTORY_H

#include "video_capture_device_factory.h"

namespace s3d {

class FileVideoCaptureDeviceFactory : public VideoCaptureDeviceFactory {
 public:
  explicit FileVideoCaptureDeviceFactory() = default;

  std::unique_ptr<VideoCaptureDevice> CreateDevice(
      const VideoCaptureDeviceDescriptor& deviceDescriptor) const override;
};

}  // namespace s3d

#endif  // S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_FACTORY_H
