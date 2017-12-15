#include "live_capture_device_factory.h"

#ifndef _WIN32
#include <s3d/video/capture/video_capture_device_decklink.h>
#endif

#include <iostream>

class FakeLiveCaptureDevice : public s3d::VideoCaptureDevice {
public:
  FakeLiveCaptureDevice() {
    std::cout << "DeckLink devices are not yet supported on Windows" << std::endl;
  }
  gsl::owner<FakeLiveCaptureDevice*> clone() const {
    return new FakeLiveCaptureDevice;
  }
  void AllocateAndStart(const s3d::VideoCaptureFormat& format, Client* client) override {}
  void StopAndDeAllocate() override {}
  s3d::VideoCaptureFormat DefaultFormat() override { return {}; }
};

std::unique_ptr<s3d::VideoCaptureDevice> LiveCaptureDeviceFactory::create() {
  #ifdef _WIN32
  return std::make_unique<FakeLiveCaptureDevice>();
  #else
  return std::make_unique<s3d::VideoCaptureDeviceDecklink>(s3d::VideoCaptureDeviceDescriptor({}));
  #endif
}
