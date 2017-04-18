//
// Created by jon on 23/03/17.
//

#ifndef VIDEO_TEXTURE_TEXTUREUPDATECLIENT_H
#define VIDEO_TEXTURE_TEXTUREUPDATECLIENT_H

#include "DynamicTextureThreadSafe.hpp"

#include "s3d/video/capture/video_capture_device_3d.h"

class TextureUpdateClient : public VideoCaptureDevice3D::Client {
 public:
  using time_point = std::chrono::time_point<std::chrono::system_clock>;

  gsl::owner<VideoCaptureDevice3D::Client*> clone() override {
    return new TextureUpdateClient(videoTexture, videoTextureR);
  }

  TextureUpdateClient(DynamicTextureThreadSafe* videoTexture,
                      DynamicTextureThreadSafe* videoTextureR)
      : videoTexture{videoTexture}, videoTextureR{videoTextureR} {}

  void OnIncomingCapturedData(gsl::span<const uint8_t> leftImage,
                              gsl::span<const uint8_t> rightImage,
                              const VideoCaptureFormat& /*frameFormat*/) override {
    outputPerformanceMetrics(std::cout);
    videoTexture->updateImage(leftImage);
    videoTextureR->updateImage(rightImage);
  }

  void outputPerformanceMetrics(std::ostream& outStream) {
    using std::chrono::milliseconds;
    using std::chrono::duration_cast;
    auto now = std::chrono::high_resolution_clock::now();
    outStream << duration_cast<milliseconds>(now - lastTimeMesure).count() << std::endl;
    lastTimeMesure = now;
  }

  void OnError(const std::string& /*unused*/) override {}
  void OnLog(const std::string& /*unused*/) override {}
  void OnStarted() override {}

 private:
  time_point lastTimeMesure;
  DynamicTextureThreadSafe* videoTexture;
  DynamicTextureThreadSafe* videoTextureR;
};

#endif  // VIDEO_TEXTURE_TEXTUREUPDATECLIENT_H
