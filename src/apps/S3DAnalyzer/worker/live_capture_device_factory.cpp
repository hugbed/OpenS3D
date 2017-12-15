#include "live_capture_device_factory.h"

#ifdef __linux__
#include <s3d/video/capture/video_capture_device_decklink.h>
#endif

#include <iostream>

class FakeLiveCaptureDevice : public s3d::VideoCaptureDevice {
public:
  FakeLiveCaptureDevice() {
    std::cout << "DeckLink devices are currently only supported on Linux" << std::endl;
  }
  gsl::owner<FakeLiveCaptureDevice*> clone() const override {
    return new FakeLiveCaptureDevice;
  }
  void AllocateAndStart(const s3d::VideoCaptureFormat& format, Client* client) override {}
  void StopAndDeAllocate() override {}
  s3d::VideoCaptureFormat DefaultFormat() override { return {}; }
};

std::unique_ptr<s3d::VideoCaptureDevice> LiveCaptureDeviceFactory::create() {
  #ifdef __linux__
  return std::make_unique<s3d::VideoCaptureDeviceDecklink>(s3d::VideoCaptureDeviceDescriptor({}));
  #else
  return std::make_unique<FakeLiveCaptureDevice>();
  #endif
}
