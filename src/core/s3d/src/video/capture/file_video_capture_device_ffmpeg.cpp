#include "s3d/video/capture/file_video_capture_device_ffmpeg.h"

#include "s3d/video/file_parser/ffmpeg/video_file_parser_ffmpeg.h"

FileVideoCaptureDeviceFFmpeg::FileVideoCaptureDeviceFFmpeg(const std::string& filename)
    : filename_(filename), FileVideoCaptureDevice(filename) {}

gsl::owner<FileVideoCaptureDeviceFFmpeg*> FileVideoCaptureDeviceFFmpeg::clone() const {
  return new FileVideoCaptureDeviceFFmpeg(filename_);
}

std::unique_ptr<VideoFileParser> FileVideoCaptureDeviceFFmpeg::GetVideoFileParser(
    const std::string& filePath,
    VideoCaptureFormat* format) {
  auto fileParser =
      std::unique_ptr<VideoFileParser>(std::make_unique<VideoFileParserFFmpeg>(filePath));

  if (!fileParser->Initialize(format)) {
    fileParser.reset();
  }

  return fileParser;
}
