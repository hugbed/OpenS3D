//
// Created by jon on 07/04/17.
//

#include "s3d/video/capture/file_video_capture_device_factory.h"
#include "s3d/video/capture/file_video_capture_device.h"

#include <iostream>

class VideoTestClient : public VideoCaptureDevice::Client {
 public:
  virtual void OnIncomingCapturedData(const std::vector<uint8_t>& data,
                                      const VideoCaptureFormat& frameFormat) {
    std::cout << "received image, size: " << data.size() << std::endl;
  }

  virtual void OnError(const std::string& reason) override {}
  virtual void OnLog(const std::string& message) override {}
  virtual void OnStarted() override {}
};

std::unique_ptr<VideoCaptureDeviceFactory> createVideoCaptureDeviceFactory() {
  return std::unique_ptr<VideoCaptureDeviceFactory>(
      new FileVideoCaptureDeviceFactory);
}

int main() {
  VideoCaptureFormat format;

  auto deviceFactory = createVideoCaptureDeviceFactory();
  auto device = deviceFactory->CreateDevice(
      std::string("/home/jon/Videos/current-left.yuv"));
  auto client =
      std::unique_ptr<VideoCaptureDevice::Client>(new VideoTestClient);

  device->AllocateAndStart({}, std::move(client));

  while (1) {
  }
}
