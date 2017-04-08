//
// Created by jon on 07/04/17.
//

#include "s3d/video/capture/FileVideoCaptureDeviceFactory.h"
#include "s3d/video/capture/FileVideoCaptureDevice.h"

std::unique_ptr<VideoCaptureDevice> FileVideoCaptureDeviceFactory::CreateDevice(
    const VideoCaptureDeviceDescriptor& deviceDescriptor) {
  return std::unique_ptr<VideoCaptureDevice>(
      new FileVideoCaptureDevice(deviceDescriptor.name));
}