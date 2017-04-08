//
// Created by bedh2102 on 06/04/17.
//

#ifndef PROJECT_FILEVIDEOCAPTUREDEVICE_H
#define PROJECT_FILEVIDEOCAPTUREDEVICE_H

#include "video_capture_device.h"

#include <thread>
#include <atomic>

class VideoFileParser;

class FileVideoCaptureDevice : public VideoCaptureDevice {
 public:
  FileVideoCaptureDevice(const std::string& filePath);

  ~FileVideoCaptureDevice() override;

  virtual void AllocateAndStart(
      const VideoCaptureFormat& format,
      std::unique_ptr<VideoCaptureDevice::Client> client) override;

  virtual void StopAndDeAllocate() override;

  // todo: should be private
  static std::unique_ptr<VideoFileParser> GetVideoFileParser(
      const std::string& filePath,
      VideoCaptureFormat& format);

 private:
  // runs on capture thread
  void OnAllocateAndStart();
  void OnCaptureTask();

  // belongs to capture thread
  std::unique_ptr<std::thread> captureThread_;
  std::unique_ptr<VideoFileParser> fileParser_;
  std::unique_ptr<Client> client_;
  std::string filePath_;
  std::vector<uint8_t> videoFrame_;
  VideoCaptureFormat captureFormat_;
  std::atomic<bool> stopCaptureFlag_;
};

#endif  // PROJECT_FILEVIDEOCAPTUREDEVICE_H
