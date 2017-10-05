#include "s3d/cv/video/stereo_demuxer/stereo_demuxer_cv_above_below.h"

#include "s3d/cv/utilities/cv.h"

namespace s3d {
StereoDemuxerCVAboveBelow::StereoDemuxerCVAboveBelow(Size frameSize,
                                                     VideoPixelFormat pixelFormat,
                                                     bool halfResolution)
    : StereoDemuxerCV(frameSize, pixelFormat, halfResolution) {}

gsl::owner<StereoDemuxerCVAboveBelow*> StereoDemuxerCVAboveBelow::clone() const {
  return new StereoDemuxerCVAboveBelow(frameSize_, pixelFormat_, halfResolution_);
}

std::pair<cv::Mat, cv::Mat> StereoDemuxerCVAboveBelow::demuxCV(const cv::Mat& originalImg) {
  auto newSize = demuxedSize();
  auto w = newSize.getWidth();
  auto h = newSize.getHeight();

  if (halfResolution_) {
    h /= 2.0f;
  }
  cv::Mat leftImg = cv::Mat(originalImg, cv::Rect(0, 0, w, h)).clone();
  cv::Mat rightImg = cv::Mat(originalImg, cv::Rect(0, h, w, h)).clone();

  if (halfResolution_) {
    resizeMat(&leftImg, 1.0f, 2.0f);
    resizeMat(&rightImg, 1.0f, 2.0f);
  }
  return {leftImg, rightImg};
}

Size StereoDemuxerCVAboveBelow::demuxedSize() const {
  if (halfResolution_) {
    return frameSize_;
  }
  return {frameSize_.getWidth(), frameSize_.getHeight() / 2};
}

Stereo3DFormat StereoDemuxerCVAboveBelow::getStereoFormat() const {
  return halfResolution_ ? Stereo3DFormat::AboveBelowHalf : Stereo3DFormat::AboveBelow;
}
}  // namespace s3d
