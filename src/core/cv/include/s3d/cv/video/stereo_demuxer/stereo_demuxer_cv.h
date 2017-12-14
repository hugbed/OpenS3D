#ifndef S3D_CV_STEREO_DEMUXER_STEREO_DEMUXER_CV_H
#define S3D_CV_STEREO_DEMUXER_STEREO_DEMUXER_CV_H

#include "s3d/video/stereo_demuxer/stereo_demuxer.h"

#include "s3d/geometry/size.h"
#include "s3d/video/video_types.h"

#include <cstdint>
#include <vector>

namespace cv {
class Mat;
}

namespace s3d {

class StereoDemuxerCV : public StereoDemuxer {
 public:
  StereoDemuxerCV(Size frameSize, VideoPixelFormat pixelFormat, bool halfResolution = false);

  std::pair<ImageData, ImageData> demux(const ImageData& image) override;
  virtual std::pair<cv::Mat, cv::Mat> demuxCV(const cv::Mat& originalImg) = 0;

  void setSize(Size size) override;
  void setPixelFormat(VideoPixelFormat format) override;

 protected:
  bool halfResolution_;
  Size frameSize_;
  VideoPixelFormat pixelFormat_;
};
}  // namespace s3d

#endif  // S3D_CV_STEREO_DEMUXER_STEREO_DEMUXER_CV_H
