// Copyright 2014 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_DECKLINK_H
#define S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_DECKLINK_H

#include "video_capture_device.h"
#include "video_capture_types.h"
#include "video_capture_device_factory.h"

#include <gsl/gsl>
#include <chrono>

class DeckLinkCaptureDelegate;

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
  void OnIncomingCapturedData(gsl::span<const uint8_t> data,
                              const VideoCaptureFormat& frameFormat);
  //std::chrono::high_resolution_clock::duration<std::chrono::high_resolution_clok::microseconds> timestamp);  // todo: include timestamp

  void SendErrorString(const std::string& /*reason*/);
  void SendLogString(const std::string& /*message*/);

 private:
  std::unique_ptr<VideoCaptureDevice::Client> client_{};
  std::unique_ptr<DeckLinkCaptureDelegate> captureDelegate_;
};

#endif  // S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_DECKLINK_H
