#ifndef PROJECT_VIDEOFILEPARSERFFMPEG_H
#define PROJECT_VIDEOFILEPARSERFFMPEG_H

#include "Demuxer.h"

#include <string>
#include <vector>

class Decoder;

class VideoFileParserFFmpeg {
 public:
  explicit VideoFileParserFFmpeg(const std::string& inputFilename);
  ~VideoFileParserFFmpeg();

  bool GetNextFrame(std::vector<uint8_t>& imageData);

 private:
  Demuxer demuxer_;
  std::unique_ptr<Decoder> decoder_;
};

#endif  // PROJECT_VIDEOFILEPARSERFFMPEG_H
