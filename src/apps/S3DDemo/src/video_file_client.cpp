//
// Created by jon on 07/04/17.
//

#include "s3d/video/capture/file_video_capture_device.h"
#include "s3d/video/capture/file_video_capture_device_factory.h"

#include <iostream>

class VideoTestClient : public VideoCaptureDevice::Client {
 public:
  gsl::owner<VideoCaptureDevice::Client*> clone() override { return new VideoTestClient(); }

  void OnIncomingCapturedData(const gsl::span<const uint8_t> imageBytes,
                              const VideoCaptureFormat& /*frameFormat*/) override {
    std::cout << "received image, size: " << imageBytes.size() << std::endl;
  }

  void OnError(const std::string& /*reason*/) override {}
  void OnLog(const std::string& /*reason*/) override {}
  void OnStarted() override {}
};

std::unique_ptr<VideoCaptureDeviceFactory> createVideoCaptureDeviceFactory() {
  return std::unique_ptr<VideoCaptureDeviceFactory>(new FileVideoCaptureDeviceFactory);
}

int main() {
  VideoCaptureFormat format;

  auto deviceFactory = createVideoCaptureDeviceFactory();
  auto device = deviceFactory->CreateDevice(
      VideoCaptureDeviceDescriptor("/home/jon/Videos/current-left.yuv"));
  auto client = std::unique_ptr<VideoCaptureDevice::Client>(new VideoTestClient);

  device->AllocateAndStart({}, std::move(client));

  while (true) {
    using std::literals::chrono_literals::operator""s;
    std::this_thread::sleep_for(1s);
  }
}
