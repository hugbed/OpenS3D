#ifndef PROJECT_VIDEOFILEPARSERFFMPEG_H
#define PROJECT_VIDEOFILEPARSERFFMPEG_H

#include "Demuxer.h"

#include <string>
#include <vector>

class Decoder;
class Scaler;

// todo: decoder->createFrameScaler() // de-align, YUV to RGB
//class FrameScaler;

class VideoFileParserFFmpeg {
 public:
  explicit VideoFileParserFFmpeg(const std::string& inputFilename);
  ~VideoFileParserFFmpeg();

  bool GetNextFrame(std::vector<uint8_t>& imageData);

 private:
  Demuxer demuxer_;
  std::unique_ptr<Decoder> decoder_;
  std::unique_ptr<Scaler> scaler_;
};

#endif  // PROJECT_VIDEOFILEPARSERFFMPEG_H
