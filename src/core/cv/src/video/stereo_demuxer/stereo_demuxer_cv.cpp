#include "s3d/cv/video/stereo_demuxer/stereo_demuxer_cv.h"

#include "s3d/cv/utilities/cv.h"

#include <utility>

namespace s3d {

StereoDemuxerCV::StereoDemuxerCV(Size frameSize, VideoPixelFormat pixelFormat, bool halfResolution)
    : halfResolution_{halfResolution}, frameSize_{frameSize}, pixelFormat_{pixelFormat} {}

void StereoDemuxerCV::demux(
    const InputImageData& image,
    OutputImageData* leftImage,
    OutputImageData* rightImage) {
  cv::Mat originalImg = dataToMat(frameSize_, pixelFormat_, image);

  cv::Mat leftImg, rightImg;
  std::tie(leftImg, rightImg) = demuxCV(originalImg);
  assert(leftImg.cols == rightImg.cols);
  assert(leftImg.rows == rightImg.rows);

  size_t nbBytes = VideoFrame::AllocationSize(pixelFormat_, Size(leftImg.cols, leftImg.rows));
  leftImage->resize(nbBytes);
  rightImage->resize(nbBytes);
  leftImage->assign(leftImg.data, leftImg.data + nbBytes);
  rightImage->assign(rightImg.data, rightImg.data + nbBytes);
}

void StereoDemuxerCV::setSize(Size size) {
  frameSize_ = size;
}

void StereoDemuxerCV::setPixelFormat(VideoPixelFormat format) {
  pixelFormat_ = format;
}

}  // namespace s3d
