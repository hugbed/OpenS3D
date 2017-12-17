#include "stereo_demuxer_qimage.h"

StereoDemuxerQImage::StereoDemuxerQImage(bool halfResolution) : halfResolution_{halfResolution} {}

void StereoDemuxerQImage::demux(
    const InputImageData& /*image*/,
    OutputImageData* /*leftImage*/,
    OutputImageData* /*rightImage*/) {
  // data to QImage to data
  // todo: not implemented
}

void StereoDemuxerQImage::setSize(s3d::Size size) {
  frameSize_ = size;
}

void StereoDemuxerQImage::setPixelFormat(s3d::VideoPixelFormat /*format*/) {}
