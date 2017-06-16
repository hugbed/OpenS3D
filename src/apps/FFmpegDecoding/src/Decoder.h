#ifndef PROJECT_DECODER_H
#define PROJECT_DECODER_H

#include <cstdint>
#include <vector>

#include "ffmpeg_utils.h"

class Decoder {
 public:
  explicit Decoder(AVFormatContext* formatContext);
  ~Decoder();

  bool sendPacketForDecoding(AVPacket* packet);

  bool receiveDecodedFrame(AVFrame** frame);

  bool endOfFileReached();

  void copyFrameData(AVFrame*, std::vector<uint8_t>* data);

 private:
  static int openCodexContext(ffmpeg::UniquePtr<AVCodecContext>& codecContext,
                              AVFormatContext* formatContext,
                              enum AVMediaType type);

 private:
  bool endOfFileReached_{false};

  int streamIndex_{0};
  ffmpeg::UniquePtr<AVCodecContext> codecContext_{nullptr};
  AVFormatContext* formatContext_;  // no ownership
  ffmpeg::UniquePtr<AVFrame> frame_;

  // output buffer (this is not very clean to have here...)
  int outputBufferSize_{0};
  uint8_t* outputBuffer_[4]{nullptr};
  int outputBufferLineSize_[4];
};

#endif  // PROJECT_DECODER_H
