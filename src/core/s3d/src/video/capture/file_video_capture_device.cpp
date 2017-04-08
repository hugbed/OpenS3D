//
// Created by bedh2102 on 06/04/17.
//

#include "s3d/video/capture/file_video_capture_device.h"
#include "s3d/utilities/file_io.h"

#include <chrono>

class VideoFileParser {
 public:
  explicit VideoFileParser(const std::string& filePath);

  virtual ~VideoFileParser() = default;

  virtual bool Initialize(VideoCaptureFormat& format) = 0;
  virtual bool GetNextFrame(std::vector<uint8_t>& frame) = 0;

 protected:
  std::string filePath;
  size_t frameSize;
  size_t currentByteIndex;
  size_t firstFrameIndex;
};

class RawUYVYFileParser : public VideoFileParser {
 public:
  explicit RawUYVYFileParser(const std::string& filePath);

  ~RawUYVYFileParser() override = default;

  bool Initialize(VideoCaptureFormat& format) override;
  bool GetNextFrame(std::vector<uint8_t>& frame) override;

 private:
  std::unique_ptr<std::ifstream> fileStream;
  std::vector<uint8_t> videoFrame;
};

VideoFileParser::VideoFileParser(const std::string& filePath)
    : filePath(filePath), frameSize{}, currentByteIndex{}, firstFrameIndex{} {}

RawUYVYFileParser::RawUYVYFileParser(const std::string& filePath)
    : VideoFileParser(filePath) {}

bool RawUYVYFileParser::Initialize(VideoCaptureFormat& format) {
  format.frameRate = 1.0f / 30.0f;
  format.frameSize = Size{1920, 1080};
  format.pixelFormat = VideoPixelFormat::UYVY;

  fileStream.reset(new std::ifstream{filePath, std::ios::binary});

  if (!fileStream->is_open()) {
    return false;
  }

  frameSize = format.ImageAllocationSize();
  return true;
}

bool RawUYVYFileParser::GetNextFrame(std::vector<uint8_t>& frame) {
  frame.resize(frameSize);
  return s3d::file_io::read_n_bytes(*fileStream.get(), frameSize,
                                    std::begin(frame));
}

// static
std::unique_ptr<VideoFileParser> FileVideoCaptureDevice::GetVideoFileParser(
    const std::string& filePath,
    VideoCaptureFormat& format) {
  // currently only RawUYVY supported
  auto fileParser = std::unique_ptr<VideoFileParser>(
      std::make_unique<RawUYVYFileParser>(filePath));

  if (!fileParser->Initialize(format)) {
    fileParser.reset();
  }

  return fileParser;
}

FileVideoCaptureDevice::FileVideoCaptureDevice(const std::string& filePath)
    : filePath_(filePath), stopCaptureFlag_(false) {}

FileVideoCaptureDevice::~FileVideoCaptureDevice() {
  // check that thread is not still runing
}

void FileVideoCaptureDevice::AllocateAndStart(
    const VideoCaptureFormat& format,
    std::unique_ptr<VideoCaptureDevice::Client> client) {
  client_ = std::move(client);
  stopCaptureFlag_ = false;
  fileParser_ = GetVideoFileParser(filePath_, captureFormat_);
  captureThread_.reset(
      new std::thread(&FileVideoCaptureDevice::OnAllocateAndStart, this));
  captureThread_->detach();  // todo: detach for now
}

void FileVideoCaptureDevice::OnAllocateAndStart() {
  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::milliseconds;
  using std::chrono::high_resolution_clock ;

  auto loopDuration = duration_cast<milliseconds>(
      duration<float>(1.0f / captureFormat_.frameRate / 1000.0f));

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
  if (fileParser_->GetNextFrame(videoFrame_)) {
    client_->OnIncomingCapturedData(videoFrame_, captureFormat_);
  } else {
    stopCaptureFlag_ = true;
  }
}