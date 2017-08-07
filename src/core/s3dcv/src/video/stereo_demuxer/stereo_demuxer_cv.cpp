#include "s3d/cv/video/stereo_demuxer/stereo_demuxer_cv.h"

#include "s3d/cv/utilities/cv.h"

namespace s3d {

StereoDemuxerCV::StereoDemuxerCV(Size frameSize, VideoPixelFormat pixelFormat, bool halfResolution)
    : frameSize_{frameSize}, pixelFormat_{pixelFormat}, halfResolution_{halfResolution} {}

std::pair<StereoDemuxer::ImageData, StereoDemuxer::ImageData> StereoDemuxerCV::demux(
    const StereoDemuxer::ImageData& image) {
  cv::Mat originalImg = dataToMat(frameSize_, pixelFormat_, image);

  cv::Mat leftImg, rightImg;
  std::tie(leftImg, rightImg) = demuxCV(originalImg);
  assert(leftImg.cols == rightImg.cols);
  assert(leftImg.rows == rightImg.rows);

  size_t nbBytes = VideoFrame::AllocationSize(pixelFormat_, Size(leftImg.cols, leftImg.rows));
  std::vector<uint8_t> leftData(leftImg.data, leftImg.data + nbBytes);
  std::vector<uint8_t> rightData(rightImg.data, rightImg.data + nbBytes);

  return {std::move(leftData), std::move(rightData)};
}

void StereoDemuxerCV::setSize(Size size) {
  frameSize_ = size;
}

void StereoDemuxerCV::setPixelFormat(VideoPixelFormat format) {
  pixelFormat_ = format;
}

}  // namespace s3d
