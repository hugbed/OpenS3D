// Copyright 2013 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#include "s3d/video/capture/file_video_capture_device.h"
#include "s3d/video/capture/video_file_parser.h"
#include "s3d/utilities/file_io.h"
#include "s3d/utilities/time.h"

FileVideoCaptureDevice::CaptureLoopClient::CaptureLoopClient(FileVideoCaptureDevice* captureDevice)
    : captureDevice_{captureDevice} {}

gsl::owner<TimedLoop::Client*> FileVideoCaptureDevice::CaptureLoopClient::clone() const {
  return new CaptureLoopClient(captureDevice_);
}

void FileVideoCaptureDevice::CaptureLoopClient::callback() {
  captureDevice_->OnCaptureTask();
}

FileVideoCaptureDevice::FileVideoCaptureDevice(std::string filePath)
    : filePath_(std::move(filePath)) {}

gsl::owner<VideoCaptureDevice*> FileVideoCaptureDevice::clone() const {
  return new FileVideoCaptureDevice(filePath_);
}

FileVideoCaptureDevice::~FileVideoCaptureDevice() = default;

void FileVideoCaptureDevice::AllocateAndStart(const VideoCaptureFormat& format,
                                              std::unique_ptr<VideoCaptureDevice::Client> client) {
  captureFormat_ = format;
  auto captureLoopClient =
      std::unique_ptr<TimedLoop::Client>(std::make_unique<CaptureLoopClient>(this));
  auto fileParser = GetVideoFileParser(filePath_, &captureFormat_);
  Start(captureFormat_, std::move(client), std::move(captureLoopClient), std::move(fileParser),
        GetTimedLoop());
}

void FileVideoCaptureDevice::Start(const VideoCaptureFormat& format,
                                   std::unique_ptr<VideoCaptureDevice::Client> client,
                                   std::unique_ptr<TimedLoop::Client> captureLoopClient,
                                   std::unique_ptr<VideoFileParser> fileParser,
                                   std::unique_ptr<TimedLoop> timedLoop) {
  captureFormat_ = format;
  client_ = std::move(client);
  captureLoopClient_ = std::move(captureLoopClient);
  fileParser_ = std::move(fileParser);
  captureLoop_ = std::move(timedLoop);
  StartCaptureThread();
}

void FileVideoCaptureDevice::StopAndDeAllocate() {
  if (captureLoop_ != nullptr) {
    captureLoop_->stop();
  }
}

void FileVideoCaptureDevice::StartCaptureThread() {
  captureThread_ = std::make_unique<std::thread>(&FileVideoCaptureDevice::OnAllocateAndStart, this);
  captureThread_->detach();
}

void FileVideoCaptureDevice::OnAllocateAndStart() {
  // blocking loop start
  assert(captureFormat_.frameRate != 0.0f);
  auto loopDuration = s3d::seconds_to_ms(1.0f / captureFormat_.frameRate);
  captureLoop_->start(captureLoopClient_.get(),
                      std::chrono::duration_cast<std::chrono::milliseconds>(loopDuration));
}

void FileVideoCaptureDevice::OnCaptureTask() {
  if (fileParser_ != nullptr && client_ != nullptr && fileParser_->GetNextFrame(videoFrame_)) {
    client_->OnIncomingCapturedData({videoFrame_}, captureFormat_);
  } else {
    StopAndDeAllocate();
  }
}

// todo: should this be done outside the class?
// todo: should unit test this
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

std::unique_ptr<TimedLoop> FileVideoCaptureDevice::GetTimedLoop() {
  return std::make_unique<TimedLoopSleep>();
}
