//
// Created by bedh2102 on 06/04/17.
//

#ifndef S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_FACTORY_H
#define S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_FACTORY_H

#include <memory>
#include <string>

// todo(hugbed): must elaborate
struct VideoCaptureDeviceDescriptor {
  VideoCaptureDeviceDescriptor(const std::string& name) : name(name) {}
  std::string name;
};

class VideoCaptureDevice;

class VideoCaptureDeviceFactory {
 public:
  virtual std::unique_ptr<VideoCaptureDevice> CreateDevice(
      const VideoCaptureDeviceDescriptor& deviceDescriptor) = 0;
};

#endif  // S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_FACTORY_H
