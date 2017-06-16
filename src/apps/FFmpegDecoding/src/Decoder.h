#ifndef PROJECT_DECODER_H
#define PROJECT_DECODER_H

#include "ffmpeg_utils.h"

class Decoder {
 public:
  explicit Decoder(AVFormatContext* formatContext);

  bool sendPacketForDecoding(AVPacket* packet);

  bool receiveDecodedFrame(AVFrame** frame);

  bool endOfFileReached();

  size_t frameBufferSize();

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
};

#endif  // PROJECT_DECODER_H
