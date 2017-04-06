//
// Created by bedh2102 on 06/04/17.
//

#ifndef PROJECT_FILEVIDEOCAPTUREDEVICE_H
#define PROJECT_FILEVIDEOCAPTUREDEVICE_H

#include "VideoCaptureDevice.h"
#include "video_capture_types.h"

class FileVideoCaptureDevice : VideoCaptureDevice {
public:
  FileVideoCaptureDevice(const std::string &filePath);

  ~FileVideoCaptureDevice() override;

  void AllocateAndStart(const VideoCaptureFormat& format,
                        std::unique_ptr<VideoCaptureDevice::Client> client) override;
  void OnStopAndDeAllocate() override;
};


#endif //PROJECT_FILEVIDEOCAPTUREDEVICE_H
