#ifndef WORKER_STEREO_DEMUXER_QIMAGE_H
#define WORKER_STEREO_DEMUXER_QIMAGE_H

#include <s3d/video/stereo_demuxer/stereo_demuxer.h>
#include <s3d/video/stereo_demuxer/stereo_demuxer_factory.h>

#include <QImage>

class StereoDemuxerQImage : public s3d::StereoDemuxer {
 public:
  explicit StereoDemuxerQImage(bool halfResolution);

  std::pair<ImageData, ImageData> demux(const ImageData& image) override;

  virtual std::pair<QImage, QImage> demuxQImage(const QImage& image) = 0;

  void setSize(s3d::Size size) override;

  void setPixelFormat(s3d::VideoPixelFormat format) override;

 protected:
  bool halfResolution_;
  s3d::Size frameSize_;
};

#endif  // WORKER_STEREO_DEMUXER_QIMAGE_H
