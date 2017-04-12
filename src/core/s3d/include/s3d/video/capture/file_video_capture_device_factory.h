//
// Created by bedh2102 on 06/04/17.
//

#ifndef S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_FACTORY_H
#define S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_FACTORY_H

#include "video_capture_device_factory.h"

// todo(hugbed): put in s3d namespace

class FileVideoCaptureDeviceFactory : public VideoCaptureDeviceFactory {
 public:
  explicit FileVideoCaptureDeviceFactory() = default;

  std::unique_ptr<VideoCaptureDevice> CreateDevice(
      const VideoCaptureDeviceDescriptor& deviceDescriptor) override;
};

#endif  // S3D_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_FACTORY_H
