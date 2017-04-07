//
// Created by bedh2102 on 06/04/17.
//

#ifndef PROJECT_VIDEOCAPTUREDEVICEFACTORY_H
#define PROJECT_VIDEOCAPTUREDEVICEFACTORY_H

#include <string>
#include <memory>

struct VideoCaptureDeviceDescriptor {
    VideoCaptureDeviceDescriptor(const std::string &name)
      : name(name)
    {}
    std::string name;
};

class VideoCaptureDevice;

// todo: singleton?
class VideoCaptureDeviceFactory {
public:
  virtual std::unique_ptr<VideoCaptureDevice> CreateDevice(
    const VideoCaptureDeviceDescriptor& deviceDescriptor
  ) = 0;
};

#endif //PROJECT_VIDEOCAPTUREDEVICEFACTORY_H
