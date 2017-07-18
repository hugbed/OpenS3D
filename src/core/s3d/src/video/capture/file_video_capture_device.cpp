// Copyright 2013 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#include "s3d/video/capture/file_video_capture_device.h"
#include "s3d/video/file_parser/raw_uyvy_file_parser.h"
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

FileVideoCaptureDevice::~FileVideoCaptureDevice() {
  // todo: is this the correct behaviour?
  WaitUntilDone();
}

void FileVideoCaptureDevice::AllocateAndStart(const VideoCaptureFormat& format,
                                              VideoCaptureDevice::Client* client) {
  captureFormat_ = format;
  auto captureLoopClient = GetTimedLoopClient();
  auto fileParser = GetVideoFileParser(filePath_);
  InitializeFileParser(fileParser, &captureFormat_);
  Start(captureFormat_, client, std::move(captureLoopClient), std::move(fileParser),
        GetTimedLoop());
}

void FileVideoCaptureDevice::Start(const VideoCaptureFormat& format,
                                   VideoCaptureDevice::Client* client,
                                   std::unique_ptr<TimedLoop::Client> captureLoopClient,
                                   std::unique_ptr<VideoFileParser> fileParser,
                                   std::unique_ptr<TimedLoop> timedLoop) {
  captureFormat_ = format;
  client_ = client;
  captureLoopClient_ = std::move(captureLoopClient);
  fileParser_ = std::move(fileParser);
  captureLoop_ = std::move(timedLoop);
  StartCaptureThread();
}

void FileVideoCaptureDevice::WaitUntilDone() {
  if (captureThread_ != nullptr) {
    captureThread_->join();
    captureThread_.reset();
  }
}

void FileVideoCaptureDevice::StopAndDeAllocate() {
  if (captureLoop_ != nullptr) {
    captureLoop_->stop();
  }
}

void FileVideoCaptureDevice::StartCaptureThread() {
  captureThread_ = std::make_unique<std::thread>(&FileVideoCaptureDevice::OnAllocateAndStart, this);
}

void FileVideoCaptureDevice::OnAllocateAndStart() {
  // blocking loop start
  assert(captureFormat_.frameRate != 0.0f);
  auto loopDuration = s3d::seconds_to_us(1.0f / captureFormat_.frameRate);
  captureLoop_->start(captureLoopClient_.get(), loopDuration);
}

void FileVideoCaptureDevice::OnCaptureTask() {
  if (fileParser_ != nullptr && client_ != nullptr && fileParser_->GetNextFrame(videoFrame_)) {
    client_->OnIncomingCapturedData({videoFrame_}, captureFormat_,
                                    std::chrono::microseconds(0));  // todo: implement timestamp
  } else {
    StopAndDeAllocate();
  }
}

// todo: should this be done outside the class?
// todo: should unit test this
std::unique_ptr<VideoFileParser> FileVideoCaptureDevice::GetVideoFileParser(
    const std::string& filePath) {
  // RawUYVY is default
  return std::unique_ptr<VideoFileParser>(std::make_unique<RawUYVYFileParser>(filePath));
}

bool FileVideoCaptureDevice::InitializeFileParser(std::unique_ptr<VideoFileParser>& fileParser, VideoCaptureFormat *format) {
  if (fileParser == nullptr || !fileParser->Initialize(format)) {
    fileParser.reset();
    return false;
  }
  return true;
}

std::unique_ptr<TimedLoop> FileVideoCaptureDevice::GetTimedLoop() {
  return std::make_unique<TimedLoopSleep>();
}

std::unique_ptr<TimedLoop::Client>  FileVideoCaptureDevice::GetTimedLoopClient() {
  return std::unique_ptr<TimedLoop::Client>(std::make_unique<CaptureLoopClient>(this));
}

VideoCaptureFormat FileVideoCaptureDevice::DefaultFormat() {
  VideoCaptureFormat format{};
  auto parser = GetVideoFileParser(filePath_);
  InitializeFileParser(parser, &format);
  return format;
}

void FileVideoCaptureDevice::MaybeSuspend() {
  VideoCaptureDevice::MaybeSuspend();
  captureLoop_->maybePause();
}

void FileVideoCaptureDevice::Resume() {
  VideoCaptureDevice::Resume();
  captureLoop_->resume();
}
