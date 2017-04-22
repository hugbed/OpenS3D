// Copyright 2013 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#ifndef S3D_VIDEO_CAPTURE_VIDEO_FILE_PARSER_H
#define S3D_VIDEO_CAPTURE_VIDEO_FILE_PARSER_H

#include "s3d/utilities/rule_of_five.h"

#include <memory>
#include <string>
#include <vector>

class VideoCaptureFormat;

class VideoFileParser : rule_of_five_interface<VideoFileParser> {
 public:
  explicit VideoFileParser(std::string filePath);

  virtual bool Initialize(VideoCaptureFormat* format) = 0;
  virtual bool GetNextFrame(std::vector<uint8_t>& frame) = 0;

 protected:
  std::string filePath_;
  size_t frameSize_;
  size_t currentByteIndex_;
  size_t firstFrameIndex_;
};

// Reads UYVY, outputs RGB
class RawUYVYFileParser : public VideoFileParser {
 public:
  explicit RawUYVYFileParser(std::string filePath);

  gsl::owner<RawUYVYFileParser*> clone() const override;

  ~RawUYVYFileParser() override;

  bool Initialize(VideoCaptureFormat* format) override;
  bool GetNextFrame(std::vector<uint8_t>& frame) override;

 private:
  std::unique_ptr<std::ifstream> fileStream_;
  std::vector<uint8_t> frameUYVY_;
};

#endif  // S3D_VIDEO_CAPTURE_VIDEO_FILE_PARSER_H
