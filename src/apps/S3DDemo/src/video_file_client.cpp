#include "s3d/video/capture/file_video_capture_device_factory.h"
#include "s3d/video/capture/file_video_capture_device_raw_uyvy.h"

#include <iostream>

using s3d::FileVideoCaptureDeviceFactory;
using s3d::VideoCaptureDevice;
using s3d::VideoCaptureDeviceDescriptor;
using s3d::VideoCaptureDeviceFactory;
using s3d::VideoCaptureFormat;

class VideoTestClient : public VideoCaptureDevice::Client {
 public:
  gsl::owner<VideoCaptureDevice::Client*> clone() const override { return new VideoTestClient; }

  void OnIncomingCapturedData(const Images& data,
                              const VideoCaptureFormat& /*frameFormat*/,
                              std::chrono::microseconds /*timestamp*/) override {
    std::cout << "Image arrived, size=" << data.size() << " bytes." << std::endl;
  }
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

  device->AllocateAndStart({}, client.get());

  while (true) {
    using std::literals::chrono_literals::operator""s;
    std::this_thread::sleep_for(1s);
  }
}
