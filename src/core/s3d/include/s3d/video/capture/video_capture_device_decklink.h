// Copyright 2014 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_DECKLINK__H
#define S_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_DECKLINK__H

#include "video_capture_device.h"
#include "video_capture_types.h"
#include "video_capture_device_factory.h"

#include <gsl/gsl>
#include <chrono>

class DecklinkCaptureDelegate;

// todo: this is mock implementation, implement it for real
class VideoCaptureDeviceDecklink : public VideoCaptureDevice {
 public:
  explicit VideoCaptureDeviceDecklink(const VideoCaptureDeviceDescriptor& deviceDescriptor);
  gsl::owner<VideoCaptureDevice*> clone() override;

  ~VideoCaptureDeviceDecklink() override;

  void AllocateAndStart(const VideoCaptureFormat& format,
                        std::unique_ptr<VideoCaptureDevice::Client> client) override;

  void StopAndDeAllocate() override;

  // called from delegate
  void OnIncomingCapturedData(const VideoCaptureDevice::Client::Images& images,
                              const VideoCaptureFormat& frameFormat);
  // std::chrono::high_resolution_clock::duration<std::chrono::high_resolution_clok::microseconds>
  // timestamp);  // todo: include timestamp

  void SendErrorString(const std::string& /*reason*/);
  void SendLogString(const std::string& /*message*/);

 private:
  std::unique_ptr<VideoCaptureDevice::Client> client_{};
  std::unique_ptr<DecklinkCaptureDelegate> captureDelegate_;  // todo:reuse delegate
};

#endif  // S_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_DECKLINK__H
