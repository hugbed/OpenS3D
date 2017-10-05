#ifndef S3D_VIDEO_STEREO_DEMUXER_STEREO_DEMUXER_FACTORY_H
#define S3D_VIDEO_STEREO_DEMUXER_STEREO_DEMUXER_FACTORY_H

#include <s3d/geometry/size.h>
#include <s3d/video/video_types.h>

#include <memory>

namespace s3d {

class StereoDemuxer;

class StereoDemuxerFactory {
 public:
  virtual std::unique_ptr<StereoDemuxer> create(Stereo3DFormat stereoFormat,
                                                Size frameSize,
                                                VideoPixelFormat pixelFormat) = 0;
};
}  // namespace s3d

#endif  // S3D_VIDEO_STEREO_DEMUXER_STEREO_DEMUXER_FACTORY_H
