#ifndef S3D_CV_STEREO_DEMUXER_STEREO_DEMUXER_AB_H
#define S3D_CV_STEREO_DEMUXER_STEREO_DEMUXER_AB_H

#include "stereo_demuxer_cv.h"

namespace s3d {

class StereoDemuxerCVAboveBelow : public StereoDemuxerCV {
 public:
  StereoDemuxerCVAboveBelow(Size frameSize,
                            VideoPixelFormat pixelFormat,
                            bool halfResolution = false);

  gsl::owner<StereoDemuxerCVAboveBelow*> clone() const override;

  std::pair<cv::Mat, cv::Mat> demuxCV(const cv::Mat& originalImg) override;

  Size demuxedSize() const override;
  Stereo3DFormat getStereoFormat() const override;
};
}

#endif  // S3D_CV_STEREO_DEMUXER_STEREO_DEMUXER_AB_H
