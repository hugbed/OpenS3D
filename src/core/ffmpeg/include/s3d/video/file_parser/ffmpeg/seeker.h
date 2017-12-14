#ifndef S3D_VIDEO_FILE_PARSER_FFMPEG_SEEKER_H
#define S3D_VIDEO_FILE_PARSER_FFMPEG_SEEKER_H

#include <chrono>

#include <gsl/gsl>

struct AVFormatContext;

namespace s3d {

class Seeker {
 public:
  Seeker(gsl::not_null<AVFormatContext*> formatContext, int streamIndex);

  void seekTo(std::chrono::microseconds timestamp);

 private:
  AVFormatContext* formatContext_;
  int streamIndex_;
};

}  // namespace s3d

#endif  // S3D_VIDEO_FILE_PARSER_FFMPEG_SEEKER_H
