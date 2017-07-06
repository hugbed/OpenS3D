// Copyright 2013 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_3D_H
#define S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_3D_H

#include "video_capture_device.h"

#include <atomic>
#include <utility>
#include <thread>

class FileParserProducer;
class FileParserConsumer;
class ProducerConsumerSynchronizer;

class FileVideoCaptureDevice3D : public VideoCaptureDevice {
 public:
  explicit FileVideoCaptureDevice3D(const std::string& filePathsStr);

  gsl::owner<VideoCaptureDevice*> clone() const override;

  ~FileVideoCaptureDevice3D() override;

  void AllocateAndStart(const VideoCaptureFormat& format, Client* client) override;

  void MaybeSuspend() override;
  void Resume() override;
  void WaitUntilDone();
  void StopAndDeAllocate() override;
  void RequestRefreshFrame() override;
  VideoCaptureFormat DefaultFormat() override;

 protected:
  void Allocate();
  void Start();

 private:
  std::pair<std::string, std::string> filePaths_;
  VideoCaptureFormat captureFormat_;

  Client* client_;
  std::unique_ptr<FileParserConsumer> consumer_;
  std::pair<std::unique_ptr<FileParserProducer>,
            std::unique_ptr<FileParserProducer>>
      producers_;

  std::unique_ptr<ProducerConsumerSynchronizer> sync_;
  std::unique_ptr<std::thread> captureThread_{nullptr};
};

#endif  // S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_3D_H
