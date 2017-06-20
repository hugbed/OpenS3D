// Copyright 2014 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_DECKLINK_H
#define S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_DECKLINK_H

#include "video_capture_device.h"
#include "video_capture_device_factory.h"

#include "gsl/gsl"

#include <chrono>

class DecklinkCaptureDelegate;

class VideoCaptureDeviceDecklink : public VideoCaptureDevice {
 public:
  explicit VideoCaptureDeviceDecklink(const VideoCaptureDeviceDescriptor& deviceDescriptor);

  gsl::owner<VideoCaptureDevice*> clone() const override;

  ~VideoCaptureDeviceDecklink() override;

  void AllocateAndStart(const VideoCaptureFormat& format,
                        std::unique_ptr<VideoCaptureDevice::Client> client) override;

  void StopAndDeAllocate() override;

  // called from delegate
  void OnIncomingCapturedData(const VideoCaptureDevice::Client::Images& images,
                              const VideoCaptureFormat& frameFormat);

  VideoCaptureFormat DefaultFormat() override;

 private:
  std::unique_ptr<VideoCaptureDevice::Client> client_{};
  std::unique_ptr<DecklinkCaptureDelegate> captureDelegate_;
};

#endif  // S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_DECKLINK_H
