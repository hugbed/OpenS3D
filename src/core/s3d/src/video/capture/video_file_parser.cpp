// Copyright 2013 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#include "s3d/video/capture/video_file_parser.h"

#include "s3d/utilities/file_io.h"
#include "s3d/video/capture/video_capture_types.h"
#include "s3d/video/compression/yuv.h"
#include "s3d/video/video_frame.h"

VideoFileParser::VideoFileParser(std::string filePath)
    : filePath_(std::move(filePath)), frameSize_{}, currentByteIndex_{}, firstFrameIndex_{} {}

RawUYVYFileParser::RawUYVYFileParser(std::string filePath) : VideoFileParser(std::move(filePath)) {}

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

  fileStream_ = std::make_unique<std::ifstream>(filePath_, std::ios::binary);
  return fileStream_->is_open();
}

bool RawUYVYFileParser::GetNextFrame(std::vector<uint8_t>& frame) {
  frame.resize(frameSize_);
  auto res = s3d::file_io::read_n_bytes(*fileStream_, frameUYVY_.size(), std::begin(frameUYVY_));
  if (res) {
    using s3d::compression::UYVY;
    using s3d::compression::BGR;
    using s3d::compression::color_conversion;
    color_conversion<UYVY, BGR> cvt;
    cvt(std::begin(frameUYVY_), std::end(frameUYVY_), std::begin(frame));
  }

  return res;
}
