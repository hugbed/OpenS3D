#ifndef WORKER_STEREO_DEMUXER_FACTORY_QIMAGE_H
#define WORKER_STEREO_DEMUXER_FACTORY_QIMAGE_H

#include <s3d/video/stereo_demuxer/stereo_demuxer_factory.h>

#include "stereo_demuxer_qimage_above_below.h"
#include "stereo_demuxer_qimage_side_by_side.h"

class StereoDemuxerFactoryQImage : public s3d::StereoDemuxerFactory {
 public:
  std::unique_ptr<s3d::StereoDemuxer> create(s3d::Stereo3DFormat stereoFormat,
                                             s3d::Size frameSize,
                                             s3d::VideoPixelFormat pixelFormat) override;

  std::unique_ptr<StereoDemuxerQImage> createStereoDemuxerQImage(s3d::Stereo3DFormat stereoFormat);
};

#endif  // WORKER_STEREO_DEMUXER_FACTORY_QIMAGE_H
