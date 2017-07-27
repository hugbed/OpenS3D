#ifndef S3D_VIDEO_FILE_PARSER_FFMPEG_VIDEO_FILE_PARSER_FFMPEG_H
#define S3D_VIDEO_FILE_PARSER_FFMPEG_VIDEO_FILE_PARSER_FFMPEG_H

#include <s3d/video/file_parser/video_file_parser.h>

#include "demuxer.h"

#include <string>
#include <vector>

class Decoder;
class Seeker;
class Scaler;
class VideoCaptureFormat;

class VideoFileParserFFmpeg : public VideoFileParser {
 public:
  explicit VideoFileParserFFmpeg(const std::string& inputFilename);

  gsl::owner<VideoFileParserFFmpeg*> clone() const override;

  ~VideoFileParserFFmpeg() override;

  bool Initialize(VideoCaptureFormat* format) override;
  bool GetNextFrame(std::vector<uint8_t>* imageData) override;
  void SeekToFrame(std::chrono::microseconds timestamp) override;
  std::chrono::microseconds CurrentFrameTimestamp() override;
  std::chrono::microseconds VideoDuration() override;

 private:
  int timestampToFrameNumber(std::chrono::microseconds timestamp, float fps);
  std::chrono::microseconds frameNumberToTimestamp(int frameNumber, float fps);

  std::string filename_;
  std::chrono::microseconds currentTimestamp_;
  size_t currentFrameID_{0};
  std::chrono::microseconds duration_;

  Demuxer demuxer_;
  std::unique_ptr<Decoder> decoder_;
  std::unique_ptr<Scaler> scaler_;
  std::unique_ptr<Seeker> seeker_;
};

#endif  // S3D_VIDEO_FILE_PARSER_FFMPEG_VIDEO_FILE_PARSER_FFMPEG_H
