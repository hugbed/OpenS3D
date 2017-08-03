#include "s3d/video/file_parser/raw_uyvy_file_parser.h"

#include "s3d/utilities/file_io.h"
#include "s3d/video/capture/video_capture_types.h"
#include "s3d/video/compression/yuv.h"
#include "s3d/video/video_frame.h"

namespace s3d {

RawUYVYFileParser::RawUYVYFileParser(std::string filePath) : filePath_(std::move(filePath)) {}

gsl::owner<RawUYVYFileParser*> RawUYVYFileParser::clone() const {
  return new RawUYVYFileParser(filePath_);
}

RawUYVYFileParser::~RawUYVYFileParser() = default;

bool RawUYVYFileParser::Initialize(VideoCaptureFormat* format) {
  format->frameRate = 30.0f;
  format->frameSize = Size{1920, 1080};
  format->pixelFormat = VideoPixelFormat::BGR;
  frameSize_ = format->ImageAllocationSize();
  frameUYVY_.resize(VideoFrame::AllocationSize(VideoPixelFormat::UYVY, format->frameSize));
  return createStream(fileStream_);
}

bool RawUYVYFileParser::GetNextFrame(std::vector<uint8_t>* frame) {
  if (frame == nullptr) {
    return false;
  }

  frame->resize(frameSize_);
  auto res = s3d::file_io::read_n_bytes(*fileStream_, frameUYVY_.size(), std::begin(frameUYVY_));
  if (res) {
    using s3d::compression::UYVY;
    using s3d::compression::BGR;
    using s3d::compression::color_conversion;
    color_conversion<UYVY, BGR> cvt;
    cvt(std::begin(frameUYVY_), std::end(frameUYVY_), std::begin(*frame));
  }
  return res;
}

const std::string& RawUYVYFileParser::getFilePath() const {
  return filePath_;
}

bool RawUYVYFileParser::createStream(std::unique_ptr<std::istream>& stream) {
  auto fileStream = std::make_unique<std::ifstream>(filePath_, std::ios::binary);
  bool res = fileStream->is_open();
  stream = std::move(fileStream);
  return res;
}

}  // namespace s3d
