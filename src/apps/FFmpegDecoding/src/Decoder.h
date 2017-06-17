#ifndef PROJECT_DECODER_H
#define PROJECT_DECODER_H

#include <cstdint>
#include <vector>

#include "ffmpeg_utils.h"

class Scaler;

class Decoder {
 public:
  explicit Decoder(AVFormatContext* formatContext);

  bool sendPacketForDecoding(AVPacket* packet);

  bool receiveDecodedFrame(AVFrame** frame);

  bool endOfFileReached();

  void copyFrameData(AVFrame*, std::vector<uint8_t>* data);

  std::unique_ptr<Scaler> createScaler(enum AVPixelFormat dstFormat);

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

  // output buffer (is this the right place to put this?)
  // could have a FrameScaler that takes the codecContext has constructor param
  // to remove alignment and convert to RGB
  int outputBufferSize_{0};
  uint8_t* outputBuffer_[4]{nullptr};
  ffmpeg::UniquePtr<uint8_t> outputBufferPtr_{nullptr};
  int outputBufferLineSize_[4];
};

#endif  // PROJECT_DECODER_H
