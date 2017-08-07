#ifndef WORKER_STEREO_DEMUXER_QIMAGE_ABOVE_BELOW_H
#define WORKER_STEREO_DEMUXER_QIMAGE_ABOVE_BELOW_H

#include "stereo_demuxer_qimage.h"

class StereoDemuxerQImageAboveBelow : public StereoDemuxerQImage {
 public:
  explicit StereoDemuxerQImageAboveBelow(bool halfResolution);

  gsl::owner<StereoDemuxerQImageAboveBelow*> clone() const override;

  std::pair<QImage, QImage> demuxQImage(const QImage& image) override;

  s3d::Size demuxedSize() const override;

  s3d::Stereo3DFormat getStereoFormat() const override;
};

#endif  // WORKER_STEREO_DEMUXER_QIMAGE_ABOVE_BELOW_H
