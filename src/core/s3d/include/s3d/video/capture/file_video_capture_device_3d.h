// Copyright 2013 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_3D_H
#define S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_3D_H

#include "video_capture_device.h"

#include <atomic>
#include <utility>

class RawUYVY3DFileParserProducer;
class RawUYVY3DFileParserConsumer;

class FileVideoCaptureDevice3D: public VideoCaptureDevice {
 public:
  gsl::owner<FileVideoCaptureDevice3D*> clone() override {
    auto& combinedPath = filePaths_.first.append(filePaths_.second);
    return new FileVideoCaptureDevice3D(combinedPath);
  }

  ~FileVideoCaptureDevice3D() override;

  explicit FileVideoCaptureDevice3D(const std::string& filePathsStr);

  void AllocateAndStart(const VideoCaptureFormat& format, std::unique_ptr<Client> client) override;

  void StopAndDeAllocate() override;

 private:
  std::pair<std::string, std::string> filePaths_;
  VideoCaptureFormat captureFormat_;
  std::atomic<bool> stopCaptureFlag_;

  std::unique_ptr<Client> client_;
  std::unique_ptr<RawUYVY3DFileParserConsumer> consumer_;
  std::pair<std::unique_ptr<RawUYVY3DFileParserProducer>,
            std::unique_ptr<RawUYVY3DFileParserProducer>>
      producers_;
};

#endif  // S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_3D_H
