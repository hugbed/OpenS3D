#include "s3d/video/capture/ffmpeg/file_video_capture_device_ffmpeg.h"

#include "s3d/video/file_parser/ffmpeg/video_file_parser_ffmpeg.h"

namespace s3d {

FileVideoCaptureDeviceFFmpeg::FileVideoCaptureDeviceFFmpeg(const std::string& filename)
    : FileVideoCaptureDeviceRawUYVY(filename), filename_(filename) {}

gsl::owner<FileVideoCaptureDeviceFFmpeg*> FileVideoCaptureDeviceFFmpeg::clone() const {
  return new FileVideoCaptureDeviceFFmpeg(filename_);
}

std::unique_ptr<VideoFileParser> FileVideoCaptureDeviceFFmpeg::GetVideoFileParser(
    const std::string& filePath) {
  return std::unique_ptr<VideoFileParser>(std::make_unique<VideoFileParserFFmpeg>(filePath));
}

}  // namespace s3d
