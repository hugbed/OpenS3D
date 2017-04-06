//
// Created by bedh2102 on 06/04/17.
//

#ifndef PROJECT_VIDEOCAPTUREDEVICE_H
#define PROJECT_VIDEOCAPTUREDEVICE_H

#include "s3d/capture/"

class VideoCaptureDevice {
public:
  class Client {
    virtual ~Client() {}

    virtual void OnIncomingCapturedData(std::vector<uint8_t> &data,
                                        const VideoCaptureFormat& frameFormat) = 0;

    virtual void OnError(const std::string& reason) = 0;
    virtual void OnLog(const std::string& message) = 0;
    virtual void OnStarted() = 0;
  };

  virtual ~VideoCaptureDevice() {}

  virtual void AllocateAndStart(const VideoCaptureFormat& format,
                                std::unique_ptr<Client> client) = 0;

  virtual void RequestRefreshFrame() {}

  virtual void MaybeSuspend() {}

  virtual void Resume() {}

  virtual void StopAndDeAllocate() = 0;
};

#endif //PROJECT_VIDEOCAPTUREDEVICE_H
