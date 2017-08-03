#ifndef S3D_VIDEO_STEREO_DEMUXER_STEREO_DEMUXER_H
#define S3D_VIDEO_STEREO_DEMUXER_STEREO_DEMUXER_H

#include "s3d/utilities/rule_of_five.h"

#include "s3d/video/capture/video_capture_types.h"
#include "s3d/video/video_types.h"

#include <cstdint>
#include <vector>

namespace s3d {

class StereoDemuxer : public rule_of_five_interface<StereoDemuxer> {
 public:
  using ImageData = std::vector<uint8_t>;
  virtual std::pair<ImageData, ImageData> demux(const ImageData& image) = 0;
  virtual Size demuxedSize() const = 0;
  virtual Stereo3DFormat getStereoFormat() const = 0;
  virtual void setSize(Size size) = 0;
  virtual void setPixelFormat(VideoPixelFormat format) = 0;
};
}

#endif  // S3D_VIDEO_STEREO_DEMUXER_STEREO_DEMUXER_H
