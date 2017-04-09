//
// Created by bedh2102 on 06/04/17.
//

#ifndef S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_H
#define S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_H

#include "video_capture_types.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// todo(hugbed): put in s3d namespace

class VideoCaptureDevice {
 public:
  class Client {
   public:
    virtual ~Client() = default;

    virtual void OnIncomingCapturedData(
        const std::vector<uint8_t>& data,
        const VideoCaptureFormat& frameFormat) = 0;

    virtual void OnError(const std::string& reason) = 0;
    virtual void OnLog(const std::string& message) = 0;
    virtual void OnStarted() = 0;
  };

  virtual ~VideoCaptureDevice() = default;

  virtual void AllocateAndStart(const VideoCaptureFormat& format,
                                std::unique_ptr<Client> client) = 0;

  virtual void RequestRefreshFrame() {}

  virtual void MaybeSuspend() {}

  virtual void Resume() {}

  virtual void StopAndDeAllocate() = 0;
};

#endif  // S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_H
