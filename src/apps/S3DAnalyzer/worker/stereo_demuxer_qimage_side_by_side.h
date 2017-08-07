#ifndef WORKER_STEREO_DEMUXER_QIMAGE_SIDE_BY_SIDE_H
#define WORKER_STEREO_DEMUXER_QIMAGE_SIDE_BY_SIDE_H

#include "stereo_demuxer_qimage.h"

class StereoDemuxerQImageSideBySide : public StereoDemuxerQImage {
 public:
  explicit StereoDemuxerQImageSideBySide(bool halfResolution);

  gsl::owner<StereoDemuxerQImageSideBySide*> clone() const override;

  std::pair<QImage, QImage> demuxQImage(const QImage& image) override;

  s3d::Size demuxedSize() const override;

  s3d::Stereo3DFormat getStereoFormat() const override;
};

#endif  // WORKER_STEREO_DEMUXER_QIMAGE_SIDE_BY_SIDE_H
