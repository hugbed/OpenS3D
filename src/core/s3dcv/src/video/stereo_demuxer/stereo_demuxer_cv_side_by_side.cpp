#include "s3d/cv/video/stereo_demuxer/stereo_demuxer_cv_side_by_side.h"

#include "s3d/cv/utilities/cv.h"

namespace s3d {
StereoDemuxerCVSideBySide::StereoDemuxerCVSideBySide(Size frameSize,
                                                     VideoPixelFormat pixelFormat,
                                                     bool halfResolution)
    : StereoDemuxerCV(frameSize, pixelFormat, halfResolution) {}

gsl::owner<StereoDemuxerCVSideBySide*> StereoDemuxerCVSideBySide::clone() const {
  return new StereoDemuxerCVSideBySide(frameSize_, pixelFormat_, halfResolution_);
}

std::pair<cv::Mat, cv::Mat> StereoDemuxerCVSideBySide::demuxCV(const cv::Mat& originalImg) {
  auto newSize = demuxedSize();
  auto w = newSize.getWidth();
  auto h = newSize.getHeight();
  if (halfResolution_) {
    w /= 2.0f;
  }

  cv::Mat leftImg = cv::Mat(originalImg, cv::Rect(0, 0, w, h)).clone();
  cv::Mat rightImg = cv::Mat(originalImg, cv::Rect(w, 0, w, h)).clone();
  if (halfResolution_) {
    resizeMat(&leftImg, 2.0f, 1.0f);
    resizeMat(&rightImg, 2.0f, 1.0f);
  }
  return {leftImg, rightImg};
}

Size StereoDemuxerCVSideBySide::demuxedSize() const {
  if (halfResolution_) {
    return frameSize_;
  }
  return {frameSize_.getWidth() / 2, frameSize_.getHeight()};
}

Stereo3DFormat StereoDemuxerCVSideBySide::getStereoFormat() const {
  return halfResolution_ ? Stereo3DFormat::SideBySideHalf : Stereo3DFormat::SideBySide;
}
}
