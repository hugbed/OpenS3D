#ifndef S3D_VIDEO_FILE_PARSER_FFMPEG_VIDEO_FILE_PARSER_FFMPEG_H
#define S3D_VIDEO_FILE_PARSER_FFMPEG_VIDEO_FILE_PARSER_FFMPEG_H

#include <s3d/video/file_parser/video_file_parser.h>

#include "demuxer.h"

#include <string>
#include <vector>

namespace s3d {

class Decoder;
class Seeker;
class Scaler;
struct VideoCaptureFormat;

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
  // todo: move this somewhere else
  static int timestampToFrameNumber(std::chrono::microseconds timestamp, float fps);
  static std::chrono::microseconds frameNumberToTimestamp(int frameNumber, float fps);

  std::string filename_;
  std::chrono::microseconds currentTimestamp_;
  std::chrono::microseconds duration_;

  Demuxer demuxer_;
  std::unique_ptr<Decoder> decoder_;
  std::unique_ptr<Scaler> scaler_;
  std::unique_ptr<Seeker> seeker_;
};

}  // namespace s3d

#endif  // S3D_VIDEO_FILE_PARSER_FFMPEG_VIDEO_FILE_PARSER_FFMPEG_H
