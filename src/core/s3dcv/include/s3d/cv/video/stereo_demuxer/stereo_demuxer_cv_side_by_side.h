
#ifndef S3D_CV_STEREO_DEMUXER_STEREO_DEMUXER_SBS_H
#define S3D_CV_STEREO_DEMUXER_STEREO_DEMUXER_SBS_H

#include "stereo_demuxer_cv.h"

namespace s3d {

class StereoDemuxerCVSideBySide : public StereoDemuxerCV {
 public:
  StereoDemuxerCVSideBySide(Size frameSize,
                            VideoPixelFormat pixelFormat,
                            bool halfResolution = false);

  gsl::owner<StereoDemuxerCVSideBySide*> clone() const override;

  std::pair<cv::Mat, cv::Mat> demuxCV(const cv::Mat& originalImg) override;

  Size demuxedSize() const override;
  Stereo3DFormat getStereoFormat() const override;
};
}  // namespace s3d

#endif  // S3D_CV_STEREO_DEMUXER_STEREO_DEMUXER_SBS_H
