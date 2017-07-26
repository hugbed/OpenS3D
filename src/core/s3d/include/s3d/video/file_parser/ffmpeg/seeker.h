#ifndef S3D_VIDEO_FILE_PARSER_FFMPEG_SEEKER_H
#define S3D_VIDEO_FILE_PARSER_FFMPEG_SEEKER_H

#include "ffmpeg_utils.h"

#include <chrono>

#include <gsl/gsl>

#include <iostream>

class Seeker {
 public:
  enum class Direction { Forward, Backward };

  Seeker(gsl::not_null<AVFormatContext*> formatContext, int streamIndex)
      : formatContext_{formatContext}, streamIndex_{streamIndex} {}

  void seekTo(std::chrono::microseconds timestamp) {
    std::chrono::duration<float> seconds = timestamp - std::chrono::microseconds(0);
    float s = seconds.count();
    auto timebase = formatContext_->streams[streamIndex_]->time_base;
    int seekTimestamp = static_cast<int>(
        (s * (static_cast<float>(timebase.den)) / static_cast<float>(timebase.num)));
    ffmpeg::avformat::seek_frame(formatContext_, streamIndex_, seekTimestamp, AVSEEK_FLAG_BACKWARD);
  }

  void seekToFrame(int frameNumber, Seeker::Direction direction = Direction::Forward) {
    ffmpeg::avformat::seek_frame(formatContext_, streamIndex_, frameNumber, AVSEEK_FLAG_FRAME);
  }

 private:
  AVFormatContext* formatContext_;
  int streamIndex_;
};

#endif  // S3D_VIDEO_FILE_PARSER_FFMPEG_SEEKER_H
