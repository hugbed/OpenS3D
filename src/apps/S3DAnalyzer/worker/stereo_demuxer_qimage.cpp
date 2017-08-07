#include "stereo_demuxer_qimage.h"

StereoDemuxerQImage::StereoDemuxerQImage(bool halfResolution) : halfResolution_{halfResolution} {}

std::pair<s3d::StereoDemuxer::ImageData, s3d::StereoDemuxer::ImageData> StereoDemuxerQImage::demux(
    const s3d::StereoDemuxer::ImageData& image) {
  // data to QImage to data
  // todo: not implemented
  return {};
}

void StereoDemuxerQImage::setSize(s3d::Size size) {
  frameSize_ = size;
}

void StereoDemuxerQImage::setPixelFormat(s3d::VideoPixelFormat format) {}
