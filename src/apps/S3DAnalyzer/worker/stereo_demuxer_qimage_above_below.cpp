#include "stereo_demuxer_qimage_above_below.h"

StereoDemuxerQImageAboveBelow::StereoDemuxerQImageAboveBelow(bool halfResolution)
    : StereoDemuxerQImage(halfResolution) {}

gsl::owner<StereoDemuxerQImageAboveBelow*> StereoDemuxerQImageAboveBelow::clone() const {
  return new StereoDemuxerQImageAboveBelow(halfResolution_);
}

std::pair<QImage, QImage> StereoDemuxerQImageAboveBelow::demuxQImage(const QImage& image) {
  frameSize_ = s3d::Size(image.width(), image.height());

  auto newSize = demuxedSize();
  auto w = newSize.getWidth();
  auto h = newSize.getHeight();

  if (halfResolution_) {
    h /= 2.0f;
  }
  QImage leftImg = image.copy(QRect(0, 0, w, h));
  QImage rightImg = image.copy(QRect(0, h, w, h));

  if (halfResolution_) {
    leftImg.scaled(QSize(w, 2 * h), Qt::IgnoreAspectRatio);
    rightImg.scaled(QSize(w, 2 * h), Qt::IgnoreAspectRatio);
  }
  return {leftImg, rightImg};
}

s3d::Size StereoDemuxerQImageAboveBelow::demuxedSize() const {
  if (halfResolution_) {
    return frameSize_;
  }
  return {frameSize_.getWidth(), frameSize_.getHeight() / 2};
}

s3d::Stereo3DFormat StereoDemuxerQImageAboveBelow::getStereoFormat() const {
  return halfResolution_ ? s3d::Stereo3DFormat::AboveBelowHalf : s3d::Stereo3DFormat::AboveBelow;
}
