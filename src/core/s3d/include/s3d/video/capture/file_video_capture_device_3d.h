//
// Created by jon on 11/04/17.
//

#ifndef S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_3D_H
#define S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_3D_H

#include "video_capture_device_3d.h"

#include <atomic>
#include <utility>

namespace std {
class thread;
}

class RawUYVY3DFileParserProducer;
class RawUYVY3DFileParserConsumer;

class FileVideoCaptureDevice3D : public VideoCaptureDevice3D {
 public:
  explicit FileVideoCaptureDevice3D(std::string filePathsStr);

  ~FileVideoCaptureDevice3D() override;

  void AllocateAndStart(const VideoCaptureFormat& format,
                        std::unique_ptr<Client> client) override;

  void StopAndDeAllocate() override;

 private:
  std::pair<std::string, std::string> filePaths_;
  VideoCaptureFormat captureFormat_;
  std::atomic<bool> stopCaptureFlag_;

  std::unique_ptr<Client> client_;
  std::unique_ptr<RawUYVY3DFileParserConsumer> consumer_;
  std::pair<std::unique_ptr<RawUYVY3DFileParserProducer>,
            std::unique_ptr<RawUYVY3DFileParserProducer>>
      producers_;
};

#endif  // S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_3D_H
