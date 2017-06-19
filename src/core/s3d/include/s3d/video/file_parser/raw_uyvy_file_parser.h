#ifndef S3D_VIDEO_CAPTURE_RAW_UYVY_FILE_PARSER_H
#define S3D_VIDEO_CAPTURE_RAW_UYVY_FILE_PARSER_H

#include "video_file_parser.h"

class VideoFileFormat;

// Reads UYVY, outputs RGB
class RawUYVYFileParser : public VideoFileParser {
 public:
  explicit RawUYVYFileParser(std::string filePath);

  gsl::owner<RawUYVYFileParser*> clone() const override;

  ~RawUYVYFileParser() override;

  bool Initialize(VideoCaptureFormat* format) override;
  bool GetNextFrame(std::vector<uint8_t>& frame) override;

 private:
  std::string filePath_;
  size_t frameSize_;
  size_t currentByteIndex_;
  size_t firstFrameIndex_;

  std::unique_ptr<std::ifstream> fileStream_;
  std::vector<uint8_t> frameUYVY_;
};

#endif  // S3D_VIDEO_CAPTURE_RAW_UYVY_FILE_PARSER_H
