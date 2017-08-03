#ifndef S3D_CV_VIDEO_STEREO_DEMUXER_FACTORY_CV_H
#define S3D_CV_VIDEO_STEREO_DEMUXER_FACTORY_CV_H

#include "s3d/video/stereo_demuxer/stereo_demuxer_factory.h"

#include "stereo_demuxer_cv_side_by_side.h"
#include "stereo_demuxer_cv_above_below.h"

namespace s3d {

// todo: could have a base StereoDemuxerFactor and this one would be StereoDemuxerFactoryCV
// with virtual create()
class StereoDemuxerFactoryCV : public StereoDemuxerFactory {
 public:
  std::unique_ptr<StereoDemuxer> create(Stereo3DFormat stereoFormat,
                                        Size frameSize,
                                        VideoPixelFormat pixelFormat) override {
    switch (stereoFormat) {
      case s3d::Stereo3DFormat::Separate:
        return nullptr;
      case s3d::Stereo3DFormat::SideBySide:
        return std::make_unique<StereoDemuxerCVSideBySide>(frameSize, pixelFormat, false);
      case s3d::Stereo3DFormat::SideBySideHalf:
        return std::make_unique<StereoDemuxerCVSideBySide>(frameSize, pixelFormat, true);
      case s3d::Stereo3DFormat::AboveBelow:
        return std::make_unique<StereoDemuxerCVAboveBelow>(frameSize, pixelFormat, false);
      case s3d::Stereo3DFormat::AboveBelowHalf:
        return std::make_unique<StereoDemuxerCVAboveBelow>(frameSize, pixelFormat, true);
    }
  }
};
}

#endif  // S3D_CV_VIDEO_STEREO_DEMUXER_FACTORY_CV_H
