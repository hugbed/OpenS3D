// Copyright 2012 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_H
#define S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_H

#include "s3d/utilities/rule_of_five.h"
#include "video_capture_types.h"

#include <memory>
#include <string>
#include <vector>

class VideoCaptureDeviceAllocationException : std::runtime_error {
 public:
  explicit VideoCaptureDeviceAllocationException(std::string reason)
      : std::runtime_error(std::move(reason)) {}
};

// todo(hugbed): put in s3d namespace

class VideoCaptureDevice : rule_of_five_interface<VideoCaptureDevice> {
 public:
  class Client : rule_of_five_interface<Client> {
   public:
    using Images = std::vector<gsl::span<const uint8_t>>;

    // can accept multiple images (mono/stereo cameras)
    virtual void OnIncomingCapturedData(const Images& data,
                                        const VideoCaptureFormat& frameFormat) = 0;
  };

  virtual void AllocateAndStart(const VideoCaptureFormat& format,
                                std::unique_ptr<Client> client) = 0;

  virtual void RequestRefreshFrame() {}

  virtual void MaybeSuspend() {}

  virtual void Resume() {}

  virtual void WaitUntilDone() {}

  virtual void StopAndDeAllocate() = 0;
};

#endif  // S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_H
