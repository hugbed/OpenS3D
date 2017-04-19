#ifndef VIDEO_TEXTURE_TEXTUREUPDATECLIENT_HPP
#define VIDEO_TEXTURE_TEXTUREUPDATECLIENT_HPP

#include "DynamicTextureThreadSafe.hpp"

#include "s3d/video/capture/video_capture_device_3d.h"

class TextureUpdateClient : public VideoCaptureDevice::Client {
 public:
  using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

  gsl::owner<VideoCaptureDevice::Client*> clone() override {
    return new TextureUpdateClient(videoTexture);
  }

  TextureUpdateClient(DynamicTextureThreadSafe* videoTexture) : videoTexture{videoTexture} {}

  void OnIncomingCapturedData(gsl::span<const uint8_t> image,
                              const VideoCaptureFormat& frameFormat) override {
//    outputPerformanceMetrics(std::cout);
    videoTexture->updateImage(image);
  }

  void outputPerformanceMetrics(std::ostream& outStream) {
    using std::chrono::milliseconds;
    using std::chrono::duration_cast;
    auto now = std::chrono::high_resolution_clock::now();
    outStream << duration_cast<milliseconds>(now - lastTimeMesure).count() << std::endl;
    lastTimeMesure = now;
  }

  void OnError(const std::string& reason) override {
    std::cerr << "Video Capture Error: " << reason << std::endl;
  }
  void OnLog(const std::string& /*unused*/) override {}
  void OnStarted() override {}

 private:
  time_point lastTimeMesure;
  DynamicTextureThreadSafe* videoTexture;
};

#endif  // VIDEO_TEXTURE_TEXTUREUPDATECLIENT_HPP
