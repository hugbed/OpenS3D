#ifndef PROJECT_DEMUXER_H
#define PROJECT_DEMUXER_H

#include "ffmpeg_utils.h"

#include <cassert>

class Decoder;

class Demuxer {
 public:
  explicit Demuxer(const std::string& inputFilename);

  bool readFrame(AVPacket** packet);

  std::unique_ptr<Decoder> createDecoder();

  // seeking functions
 private:
  bool endOfFileReached_{false};
  ffmpeg::UniquePtr<AVFormatContext> formatContext_{nullptr};
  AVPacket packet_;
};

#endif  // PROJECT_DEMUXER_H
