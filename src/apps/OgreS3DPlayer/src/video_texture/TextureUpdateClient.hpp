#ifndef VIDEO_TEXTURE_TEXTUREUPDATECLIENT_HPP
#define VIDEO_TEXTURE_TEXTUREUPDATECLIENT_HPP

#include "DynamicTextureThreadSafe.hpp"

#include "s3d/video/capture/video_capture_device.h"

class TextureUpdateClient : public VideoCaptureDevice::Client {
 public:
  using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

  gsl::owner<VideoCaptureDevice::Client*> clone() override {
    return new TextureUpdateClient(videoTextures_);
  }

  explicit TextureUpdateClient(const std::vector<DynamicTextureThreadSafe*>& videoTextures)
      : videoTextures_{videoTextures} {}

  void OnIncomingCapturedData(const Images& images,
                              const VideoCaptureFormat& frameFormat) override {
    //    outputPerformanceMetrics(std::cout);

    if (!images.empty() && !videoTextures_.empty()) {
      videoTextures_[0]->updateImage(images[0]);
    }

    if (frameFormat.stereo3D && images.size() == 2 && videoTextures_.size() == 2) {
      videoTextures_[1]->updateImage(images[1]);
    }
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
  std::vector<DynamicTextureThreadSafe*> videoTextures_;
};

#endif  // VIDEO_TEXTURE_TEXTUREUPDATECLIENT_HPP
