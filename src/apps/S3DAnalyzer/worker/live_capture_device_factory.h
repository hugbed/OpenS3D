#ifndef WORKER_LIVE_CAPTURE_DEVICE_FACTORY_H
#define WORKER_LIVE_CAPTURE_DEVICE_FACTORY_H

#include <s3d/video/capture/video_capture_device.h>

#include <memory>

class FakeLiveCaptureDevice;

class LiveCaptureDeviceFactory {
public:
  std::unique_ptr<s3d::VideoCaptureDevice> create();
};

#endif //WORKER_LIVE_CAPTURE_DEVICE_FACTORY_H