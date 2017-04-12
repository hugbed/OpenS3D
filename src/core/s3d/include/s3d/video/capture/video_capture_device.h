//
// Created by bedh2102 on 06/04/17.
//

#ifndef S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_H
#define S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_H

#include "s3d/utilities/rule_of_five.h"
#include "video_capture_types.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// todo(hugbed): put in s3d namespace

class VideoCaptureDevice : rule_of_five_interface<VideoCaptureDevice> {
 public:
  class Client : rule_of_five_interface<Client> {
   public:
    virtual void OnIncomingCapturedData(
        const std::vector<uint8_t>& data,
        const VideoCaptureFormat& frameFormat) = 0;

    virtual void OnError(const std::string& reason) = 0;
    virtual void OnLog(const std::string& message) = 0;
    virtual void OnStarted() = 0;
  };

  virtual void AllocateAndStart(const VideoCaptureFormat& format,
                                std::unique_ptr<Client> client) = 0;

  virtual void RequestRefreshFrame() {}

  virtual void MaybeSuspend() {}

  virtual void Resume() {}

  virtual void StopAndDeAllocate() = 0;
  //
  //
  //  // rule of six
  //  VideoCaptureDevice(const VideoCaptureDevice&) = delete;
  //  VideoCaptureDevice& operator=(const VideoCaptureDevice&) = delete;
  //
  //  VideoCaptureDevice(VideoCaptureDevice&&) = delete;
  //  VideoCaptureDevice& operator=(VideoCaptureDevice&&) = delete;
};

#endif  // S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_H
