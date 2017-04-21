// Copyright 2013 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#include "s3d/video/capture/file_video_capture_device.h"
#include "s3d/video/capture/video_file_parser.h"
#include "s3d/utilities/file_io.h"

// static
std::unique_ptr<VideoFileParser> FileVideoCaptureDevice::GetVideoFileParser(
    const std::string& filePath,
    VideoCaptureFormat* format) {
  // currently only RawUYVY supported
  auto fileParser = std::unique_ptr<VideoFileParser>(std::make_unique<RawUYVYFileParser>(filePath));

  if (!fileParser->Initialize(format)) {
    fileParser.reset();
  }

  return fileParser;
}

FileVideoCaptureDevice::~FileVideoCaptureDevice() = default;

FileVideoCaptureDevice::FileVideoCaptureDevice(std::string filePath)
    : filePath_(std::move(filePath)), stopCaptureFlag_(false) {}

// todo(hugbed): check that thread is not still runing
// FileVideoCaptureDevice::~FileVideoCaptureDevice()  = default;

void FileVideoCaptureDevice::AllocateAndStart(const VideoCaptureFormat& /*format*/,
                                              std::unique_ptr<VideoCaptureDevice::Client> client) {
  client_ = std::move(client);
  stopCaptureFlag_ = false;

  fileParser_ = GetVideoFileParser(filePath_, &captureFormat_);
  if (fileParser_ == nullptr) {
    client_->OnError("File not found.");
    return;
  }

  captureThread_ = std::make_unique<std::thread>(&FileVideoCaptureDevice::OnAllocateAndStart, this);
  captureThread_->detach();  // todo: detach for now
}

void FileVideoCaptureDevice::OnAllocateAndStart() {
  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::milliseconds;
  using std::chrono::high_resolution_clock;

  auto loopDuration =
      duration_cast<milliseconds>(duration<float>(1.0f / captureFormat_.frameRate / 1000.0f));

  while (!stopCaptureFlag_) {
    auto t1 = high_resolution_clock::now();
    OnCaptureTask();
    auto dt = std::chrono::high_resolution_clock::now() - t1;
    std::this_thread::sleep_for(loopDuration - dt);
  }
}

void FileVideoCaptureDevice::StopAndDeAllocate() {
  stopCaptureFlag_ = true;
}

void FileVideoCaptureDevice::OnCaptureTask() {
  if (fileParser_->GetNextFrame(videoFrame_) || client_) {
    client_->OnIncomingCapturedData({videoFrame_}, captureFormat_);
  } else {
    stopCaptureFlag_ = true;
  }
}
