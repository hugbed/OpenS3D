#ifndef VIDEO_TEXTURE_TEXTUREUPDATECLIENT3D_HPP
#define VIDEO_TEXTURE_TEXTUREUPDATECLIENT3D_HPP

#include "DynamicTextureThreadSafe.hpp"

#include "s3d/video/capture/video_capture_device.h"

class TextureUpdateClient3D : public VideoCaptureDevice::Client {
 public:
  using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

  gsl::owner<VideoCaptureDevice::Client*> clone() override {
    return new TextureUpdateClient3D(videoTexture, videoTextureR);
  }

  TextureUpdateClient3D(DynamicTextureThreadSafe* videoTexture,
                        DynamicTextureThreadSafe* videoTextureR)
      : videoTexture{videoTexture}, videoTextureR{videoTextureR} {}

  void OnIncomingCapturedData(const Images& images,
                              const VideoCaptureFormat& /*frameFormat*/) override {
    //    outputPerformanceMetrics(std::cout);

    if (images.size() < 2) {
      std::cerr << "Number of images < 2 for 3D texture update" << std::endl;
      return;
    }

    videoTexture->updateImage(images[0]);
    videoTextureR->updateImage(images[1]);
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
  DynamicTextureThreadSafe* videoTextureR;
};

#endif  // VIDEO_TEXTURE_TEXTUREUPDATECLIENT3D_HPP
