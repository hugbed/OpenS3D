// Copyright 2014 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_FACTORY_H
#define S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_FACTORY_H

#include <string>

#include <memory>

// todo(hugbed): put in s3d namespace
// todo(hugbed): must elaborate
struct VideoCaptureDeviceDescriptor {
  explicit VideoCaptureDeviceDescriptor(std::string name) : name(std::move(name)) {}

  VideoCaptureDeviceDescriptor(const VideoCaptureDeviceDescriptor&) = default;

  std::string name;
};

class VideoCaptureDevice;

class VideoCaptureDeviceFactory {
 public:
  virtual std::unique_ptr<VideoCaptureDevice> CreateDevice(
      const VideoCaptureDeviceDescriptor& deviceDescriptor) = 0;
};

#endif  // S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_FACTORY_H
