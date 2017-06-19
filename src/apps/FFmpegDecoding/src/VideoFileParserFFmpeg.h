#ifndef PROJECT_VIDEOFILEPARSERFFMPEG_H
#define PROJECT_VIDEOFILEPARSERFFMPEG_H

#include <s3d/video/file_parser/video_file_parser.h>

#include "Demuxer.h"

#include <string>
#include <vector>

class Decoder;
class Scaler;
class VideoCaptureFormat;

class VideoFileParserFFmpeg : public VideoFileParser {
 public:
  explicit VideoFileParserFFmpeg(const std::string& inputFilename);

  gsl::owner<VideoFileParserFFmpeg*> clone() const override;

  ~VideoFileParserFFmpeg();

  bool Initialize(VideoCaptureFormat* format) override;
  bool GetNextFrame(std::vector<uint8_t>& imageData) override;

 private:
  std::string filename_;
  Demuxer demuxer_;
  std::unique_ptr<Decoder> decoder_;
  std::unique_ptr<Scaler> scaler_;
};

#endif  // PROJECT_VIDEOFILEPARSERFFMPEG_H
