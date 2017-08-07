#include "stereo_demuxer_qimage_side_by_side.h"

StereoDemuxerQImageSideBySide::StereoDemuxerQImageSideBySide(bool halfResolution = false)
    : StereoDemuxerQImage(halfResolution) {}

gsl::owner<StereoDemuxerQImageSideBySide*> StereoDemuxerQImageSideBySide::clone() const {
  return new StereoDemuxerQImageSideBySide(halfResolution_);
}

std::pair<QImage, QImage> StereoDemuxerQImageSideBySide::demuxQImage(const QImage& image) {
  frameSize_ = s3d::Size(image.width(), image.height());

  auto newSize = demuxedSize();
  auto w = newSize.getWidth();
  auto h = newSize.getHeight();
  if (halfResolution_) {
    w /= 2.0f;
  }

  QImage leftImg = image.copy(QRect(0, 0, w, h));
  QImage rightImg = image.copy(QRect(w, 0, w, h));
  if (halfResolution_) {
    leftImg.scaled(QSize(w * 2, h), Qt::IgnoreAspectRatio);
    rightImg.scaled(QSize(w * 2, h), Qt::IgnoreAspectRatio);
  }
  return {leftImg, rightImg};
}

s3d::Size StereoDemuxerQImageSideBySide::demuxedSize() const {
  if (halfResolution_) {
    return frameSize_;
  }
  return {frameSize_.getWidth() / 2, frameSize_.getHeight()};
}

s3d::Stereo3DFormat StereoDemuxerQImageSideBySide::getStereoFormat() const {
  return halfResolution_ ? s3d::Stereo3DFormat::SideBySideHalf : s3d::Stereo3DFormat::SideBySide;
}
