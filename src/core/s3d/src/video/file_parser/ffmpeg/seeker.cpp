#include "s3d/video/file_parser/ffmpeg/seeker.h"

#include "s3d/video/file_parser/ffmpeg/ffmpeg_utils.h"

Seeker::Seeker(gsl::not_null<AVFormatContext*> formatContext, int streamIndex)
    : formatContext_{formatContext}, streamIndex_{streamIndex} {}

void Seeker::seekTo(std::chrono::microseconds timestamp) {
  std::chrono::duration<float> seconds = timestamp - std::chrono::microseconds(0);
  float s = seconds.count();
  auto timebase = formatContext_->streams[streamIndex_]->time_base;
  int seekTimestamp =
      static_cast<int>(s * static_cast<float>(timebase.den) / static_cast<float>(timebase.num));
  ffmpeg::avformat::seek_frame(formatContext_, streamIndex_, seekTimestamp, AVSEEK_FLAG_BACKWARD);
}
