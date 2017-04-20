#ifndef VIDEO_TEXTURE_TEXTUREUPDATECLIENT_HPP
#define VIDEO_TEXTURE_TEXTUREUPDATECLIENT_HPP

#include "DynamicTextureThreadSafe.hpp"

#include "s3d/video/capture/video_capture_device.h"

class TextureUpdateClient : public VideoCaptureDevice::Client {
 public:
  using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

  gsl::owner<VideoCaptureDevice::Client*> clone() override {
    return new TextureUpdateClient(videoTexture);
  }

  TextureUpdateClient(DynamicTextureThreadSafe* videoTexture) : videoTexture{videoTexture} {}

  void OnIncomingCapturedData(const Images& images,
                              const VideoCaptureFormat& frameFormat) override {
    //    outputPerformanceMetrics(std::cout);

    if (images.size() < 1) {
      std::cerr << "No image provided to texture update client" << std::endl;
    }

    // todo: problematic if images.size() == 0
    videoTexture->updateImage(images[0]);
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
