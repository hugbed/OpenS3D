#ifndef S3D_VIDEO_FILE_PARSER_FFMPEG_SCALER_H
#define S3D_VIDEO_FILE_PARSER_FFMPEG_SCALER_H

#include "ffmpeg_utils.h"

#include <cassert>
#include <vector>

// converts from codec type to dstFormat
class Scaler {
 public:
  Scaler(AVCodecContext* codecContext, enum AVPixelFormat dstFormat);

  void scaleFrame(AVFrame* frame, std::vector<uint8_t>* out);

 private:
  AVCodecContext* codecContext_;
  ffmpeg::UniquePtr<SwsContext> swsContext_;

  // output buffer (unaligned)
  int dstBufferSize_{0};
  int dstBufferLineSize_[4];
  uint8_t* dstBuffer_[4]{nullptr};
  ffmpeg::UniquePtr<uint8_t> dstBufferPtr_{nullptr};
};

#endif  // S3D_VIDEO_FILE_PARSER_FFMPEG_SCALER_H
