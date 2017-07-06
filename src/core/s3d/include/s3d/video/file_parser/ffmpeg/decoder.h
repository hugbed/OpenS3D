#ifndef S3D_VIDEO_FILE_PARSER_FFMPEG_DECODER_H
#define S3D_VIDEO_FILE_PARSER_FFMPEG_DECODER_H

#include "ffmpeg_utils.h"

#include <s3d/geometry/size.h>

#include <chrono>
#include <cstdint>
#include <vector>

class Scaler;

class Decoder {
 public:
  explicit Decoder(AVFormatContext* formatContext);

  bool sendPacketForDecoding(AVPacket* packet);

  bool receiveDecodedFrame(AVFrame** frame);

  bool endOfFileReached();

  void copyFrameData(AVFrame*, std::vector<uint8_t>* data);

  std::unique_ptr<Scaler> createScaler(enum AVPixelFormat dstFormat);

  Size getImageSize() const;

  float getFrameRate() const;

  std::chrono::microseconds getDuration() const;

  std::chrono::microseconds getFrameTimeStamp(AVFrame* frame) const;

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

  // output buffer
  int outputBufferSize_{0};
  uint8_t* outputBuffer_[4]{nullptr};
  ffmpeg::UniquePtr<uint8_t> outputBufferPtr_{nullptr};
  int outputBufferLineSize_[4];
};

#endif  // S3D_VIDEO_FILE_PARSER_FFMPEG_DECODER_H
