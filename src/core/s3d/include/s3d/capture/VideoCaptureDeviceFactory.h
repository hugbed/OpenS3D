//
// Created by bedh2102 on 06/04/17.
//

#ifndef PROJECT_VIDEOCAPTUREDEVICEFACTORY_H
#define PROJECT_VIDEOCAPTUREDEVICEFACTORY_H

// todo: singleton?
class VideoCaptureDeviceFactory {
public:
  VideoCaptureDeviceFactory();

  virtual std::unique_ptr<VideoCaptureDevice> CreateDevice(
    const VideoCaptureDeviceDescriptor& deviceDescriptor) = 0;
};

#endif //PROJECT_VIDEOCAPTUREDEVICEFACTORY_H
