//
// Created by jon on 07/04/17.
//

#include "s3d/video/capture/file_video_capture_device_factory.h"
#include "s3d/video/capture/file_video_capture_device.h"

std::unique_ptr<VideoCaptureDevice> FileVideoCaptureDeviceFactory::CreateDevice(
    const VideoCaptureDeviceDescriptor& deviceDescriptor) {
  return std::unique_ptr<VideoCaptureDevice>(
      new FileVideoCaptureDevice(deviceDescriptor.name));
}