// Copyright 2014 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_DECKLINK_3D_H
#define S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_DECKLINK_3D_H

#include "video_capture_device_3d.h"
#include "video_capture_types.h"
#include "video_capture_device_factory.h"

#include <gsl/gsl>
#include <chrono>

class DecklinkCaptureDelegate3D;

// todo: this is mock implementation, implement it for real
class VideoCaptureDeviceDecklink3D : public VideoCaptureDevice3D {
 public:
  explicit VideoCaptureDeviceDecklink3D(const VideoCaptureDeviceDescriptor& deviceDescriptor);
  gsl::owner<VideoCaptureDevice3D*> clone() override;

  ~VideoCaptureDeviceDecklink3D() override;

  void AllocateAndStart(const VideoCaptureFormat& format,
                        std::unique_ptr<VideoCaptureDevice3D::Client> client) override;

  void StopAndDeAllocate() override;

  // called from delegate
  void OnIncomingCapturedData(gsl::span<const uint8_t> imageLeft,
                              gsl::span<const uint8_t> imageRight,
                              const VideoCaptureFormat& frameFormat);
  // std::chrono::high_resolution_clock::duration<std::chrono::high_resolution_clok::microseconds>
  // timestamp);  // todo: include timestamp

  void SendErrorString(const std::string& /*reason*/);
  void SendLogString(const std::string& /*message*/);

 private:
  std::unique_ptr<VideoCaptureDevice3D::Client> client_{};
  std::unique_ptr<DecklinkCaptureDelegate3D> captureDelegate_;  // todo:reuse delegate
};

#endif  // S3D_VIDEO_CAPTURE_VIDEO_CAPTURE_DEVICE_DECKLINK_3D_H
