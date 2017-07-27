// Copyright 2014 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_FACTORY_H
#define S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_FACTORY_H

#include <memory>
#include <string>

namespace s3d {

// todo(hugbed): must elaborate
struct VideoCaptureDeviceDescriptor {
  explicit VideoCaptureDeviceDescriptor(std::string name) : name(std::move(name)) {}

  VideoCaptureDeviceDescriptor(const VideoCaptureDeviceDescriptor&) = default;

  std::string name;
};

class VideoCaptureDevice;

// todo: not really useful for now
class VideoCaptureDeviceFactory {
 public:
  virtual std::unique_ptr<VideoCaptureDevice> CreateDevice(
      const VideoCaptureDeviceDescriptor& deviceDescriptor) const = 0;
};

}  // namespace s3d

#endif  // S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_FACTORY_H
