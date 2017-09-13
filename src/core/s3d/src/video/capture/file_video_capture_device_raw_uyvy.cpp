// Copyright 2013 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#include "s3d/video/capture/file_video_capture_device_raw_uyvy.h"

#include "s3d/utilities/file_io.h"
#include "s3d/utilities/time.h"
#include "s3d/video/file_parser/raw_uyvy_file_parser.h"

namespace s3d {

FileVideoCaptureDeviceRawUYVY::CaptureLoopClient::CaptureLoopClient(
    FileVideoCaptureDeviceRawUYVY* captureDevice)
    : captureDevice_{captureDevice} {}

gsl::owner<TimedLoop::Client*> FileVideoCaptureDeviceRawUYVY::CaptureLoopClient::clone() const {
  return new CaptureLoopClient(captureDevice_);
}

void FileVideoCaptureDeviceRawUYVY::CaptureLoopClient::callback() {
  captureDevice_->OnCaptureTask();
}

FileVideoCaptureDeviceRawUYVY::FileVideoCaptureDeviceRawUYVY(std::string filePath)
    : filePath_(std::move(filePath)) {}

gsl::owner<VideoCaptureDevice*> FileVideoCaptureDeviceRawUYVY::clone() const {
  return new FileVideoCaptureDeviceRawUYVY(filePath_);
}

FileVideoCaptureDeviceRawUYVY::~FileVideoCaptureDeviceRawUYVY() {
  // todo: is this the correct behaviour?
  WaitUntilDone();
}

void FileVideoCaptureDeviceRawUYVY::AllocateAndStart(const VideoCaptureFormat& format,
                                                     VideoCaptureDevice::Client* client) {
  captureFormat_ = format;
  auto captureLoopClient = GetTimedLoopClient();
  auto fileParser = GetVideoFileParser(filePath_);
  InitializeFileParser(fileParser, &captureFormat_);
  Start(
      captureFormat_, client, std::move(captureLoopClient), std::move(fileParser), GetTimedLoop());
}

void FileVideoCaptureDeviceRawUYVY::Start(const VideoCaptureFormat& format,
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

void FileVideoCaptureDeviceRawUYVY::WaitUntilDone() {
  if (captureThread_ != nullptr) {
    captureThread_->join();
    captureThread_.reset();
  }
}

void FileVideoCaptureDeviceRawUYVY::StopAndDeAllocate() {
  if (captureLoop_ != nullptr) {
    // if paused, resume to allow to stop
    captureLoop_->resume();
    captureLoop_->stop();
    WaitUntilDone();
  }
}

void FileVideoCaptureDeviceRawUYVY::StartCaptureThread() {
  captureThread_ =
      std::make_unique<std::thread>(&FileVideoCaptureDeviceRawUYVY::OnAllocateAndStart, this);
}

void FileVideoCaptureDeviceRawUYVY::OnAllocateAndStart() {
  // blocking loop start
  assert(captureFormat_.frameRate != 0.0f);
  auto loopDuration = s3d::seconds_to_us(1.0f / captureFormat_.frameRate);
  captureLoop_->start(captureLoopClient_.get(), loopDuration);
}

void FileVideoCaptureDeviceRawUYVY::OnCaptureTask() {
  // not two can read/seek at once
  bool frameReceived = false;
  std::chrono::microseconds timestamp = {};
  {
    std::unique_lock<std::mutex> l(seekingMutex_);
    if (shouldSeek_) {
      shouldSeek_ = false;
      fileParser_->SeekToFrame(seekingTimestamp_);
    }
    if (fileParser_ != nullptr) {
      frameReceived = fileParser_->GetNextFrame(&videoFrame_);
      timestamp = fileParser_->CurrentFrameTimestamp();
    }
  }

  if (client_ != nullptr && frameReceived) {
    client_->OnIncomingCapturedData({videoFrame_}, captureFormat_, timestamp);
  }
}

// todo: should this be done outside the class?
// todo: should unit test this
std::unique_ptr<VideoFileParser> FileVideoCaptureDeviceRawUYVY::GetVideoFileParser(
    const std::string& filePath) {
  // RawUYVY is default
  return std::unique_ptr<VideoFileParser>(std::make_unique<RawUYVYFileParser>(filePath));
}

bool FileVideoCaptureDeviceRawUYVY::InitializeFileParser(
    std::unique_ptr<VideoFileParser>& fileParser,
    VideoCaptureFormat* format) {
  if (fileParser == nullptr || !fileParser->Initialize(format)) {
    fileParser.reset();
    return false;
  }
  return true;
}

std::unique_ptr<TimedLoop> FileVideoCaptureDeviceRawUYVY::GetTimedLoop() {
  return std::make_unique<TimedLoopSleep>();
}

std::unique_ptr<TimedLoop::Client> FileVideoCaptureDeviceRawUYVY::GetTimedLoopClient() {
  return std::unique_ptr<TimedLoop::Client>(std::make_unique<CaptureLoopClient>(this));
}

VideoCaptureFormat FileVideoCaptureDeviceRawUYVY::DefaultFormat() {
  VideoCaptureFormat format{};
  auto parser = GetVideoFileParser(filePath_);
  InitializeFileParser(parser, &format);
  return format;
}

void FileVideoCaptureDeviceRawUYVY::MaybeSuspend() {
  VideoCaptureDevice::MaybeSuspend();
  captureLoop_->maybePause();
}

void FileVideoCaptureDeviceRawUYVY::Resume() {
  VideoCaptureDevice::Resume();
  captureLoop_->resume();
}

void FileVideoCaptureDeviceRawUYVY::MaybeSeekTo(std::chrono::microseconds timestamp) {
  {
    std::unique_lock<std::mutex> l{seekingMutex_};
    seekingTimestamp_ = timestamp;
    shouldSeek_ = true;
  }
  RequestRefreshFrame();
}

void FileVideoCaptureDeviceRawUYVY::RequestRefreshFrame() {
  VideoCaptureDevice::RequestRefreshFrame();
  OnCaptureTask();
}

}  // namespace s3d
