// Copyright 2013 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S3D_VIDEO_CAPTURE_VIDEO_FILE_PARSER_H
#define S3D_VIDEO_CAPTURE_VIDEO_FILE_PARSER_H

#include "s3d/utilities/rule_of_five.h"

#include <chrono>
#include <cstdint>
#include <vector>

class VideoCaptureFormat;

class VideoFileParser : rule_of_five_interface<VideoFileParser> {
 public:
  virtual bool Initialize(VideoCaptureFormat* format) = 0;
  virtual bool GetNextFrame(std::vector<uint8_t>* frame) = 0;
  virtual void SeekToFrame(std::chrono::microseconds timestamp) {}  // todo: pure virtual
  virtual std::chrono::microseconds CurrentFrameTimestamp() {
    return {};
  };                                                                 // todo: make it pure virtual?
  virtual std::chrono::microseconds VideoDuration() { return {}; };  // todo: make it pure virtual?
};

#endif  // S3D_VIDEO_CAPTURE_VIDEO_FILE_PARSER_H
