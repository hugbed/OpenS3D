#ifndef S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_FFMPEG_H
#define S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_FFMPEG_H

#include <s3d/video/capture/file_video_capture_device_raw_uyvy.h>

namespace s3d {

class FileVideoCaptureDeviceFFmpeg : public FileVideoCaptureDeviceRawUYVY {
 public:
  explicit FileVideoCaptureDeviceFFmpeg(const std::string& filename);

  gsl::owner<FileVideoCaptureDeviceFFmpeg*> clone() const override;

  std::unique_ptr<VideoFileParser> GetVideoFileParser(const std::string& filePath) override;

 private:
  std::string filename_;
};

}  // namespace s3d

#endif  // S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_FFMPEG_H
