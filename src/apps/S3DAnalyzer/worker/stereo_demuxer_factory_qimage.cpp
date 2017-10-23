#include "stereo_demuxer_factory_qimage.h"

std::unique_ptr<s3d::StereoDemuxer> StereoDemuxerFactoryQImage::create(
    s3d::Stereo3DFormat stereoFormat,
    s3d::Size /*frameSize*/,
    s3d::VideoPixelFormat /*pixelFormat*/) {
  return createStereoDemuxerQImage(stereoFormat);
}

std::unique_ptr<StereoDemuxerQImage> StereoDemuxerFactoryQImage::createStereoDemuxerQImage(
    s3d::Stereo3DFormat stereoFormat) {
  switch (stereoFormat) {
    case s3d::Stereo3DFormat::Separate:
      return nullptr;
    case s3d::Stereo3DFormat::SideBySide:
      return std::make_unique<StereoDemuxerQImageSideBySide>(false);
    case s3d::Stereo3DFormat::SideBySideHalf:
      return std::make_unique<StereoDemuxerQImageSideBySide>(true);
    case s3d::Stereo3DFormat::AboveBelow:
      return std::make_unique<StereoDemuxerQImageAboveBelow>(false);
    case s3d::Stereo3DFormat::AboveBelowHalf:
      return std::make_unique<StereoDemuxerQImageAboveBelow>(true);
  }
}
