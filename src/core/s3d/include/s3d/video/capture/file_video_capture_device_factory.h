// Copyright 2014 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_FACTORY_H
#define S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_FACTORY_H

#include "video_capture_device_factory.h"

// todo(hugbed): put in s3d namespace

class FileVideoCaptureDeviceFactory : public VideoCaptureDeviceFactory {
 public:
  explicit FileVideoCaptureDeviceFactory() = default;

  std::unique_ptr<VideoCaptureDevice> CreateDevice(
      const VideoCaptureDeviceDescriptor& deviceDescriptor) const override;
};

#endif  // S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_FACTORY_H
