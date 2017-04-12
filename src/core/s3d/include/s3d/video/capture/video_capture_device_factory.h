//
// Created by bedh2102 on 06/04/17.
//

#ifndef S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_FACTORY_H
#define S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_FACTORY_H

#include <memory>
#include <string>

// todo(hugbed): put in s3d namespace

// todo(hugbed): must elaborate
struct VideoCaptureDeviceDescriptor {
  explicit VideoCaptureDeviceDescriptor(std::string name)
      : name(std::move(name)) {}
  std::string name;
};

class VideoCaptureDevice;

class VideoCaptureDeviceFactory {
 public:
  virtual std::unique_ptr<VideoCaptureDevice> CreateDevice(
      const VideoCaptureDeviceDescriptor& deviceDescriptor) = 0;
};

#endif  // S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_FACTORY_H
