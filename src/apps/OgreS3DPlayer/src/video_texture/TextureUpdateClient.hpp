#ifndef VIDEO_TEXTURE_TEXTUREUPDATECLIENT_HPP
#define VIDEO_TEXTURE_TEXTUREUPDATECLIENT_HPP

#include "DynamicTextureThreadSafe.hpp"

#include "s3d/video/capture/video_capture_device.h"

#include <iomanip>

class TextureUpdateClient : public VideoCaptureDevice::Client {
 public:
  using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

  gsl::owner<VideoCaptureDevice::Client*> clone() const override {
    return new TextureUpdateClient(videoTextures_);
  }

  explicit TextureUpdateClient(const std::vector<DynamicTextureThreadSafe*>& videoTextures)
      : videoTextures_{videoTextures} {}

  void OnIncomingCapturedData(const Images& images,
                              const VideoCaptureFormat& frameFormat,
                              std::chrono::microseconds timestamp) override {
    //    outputPerformanceMetrics(std::cout);

    if (!images.empty() && !videoTextures_.empty()) {
      videoTextures_[0]->updateImage(images[0]);
    }

    if (frameFormat.stereo3D && images.size() == 2 && videoTextures_.size() == 2) {
      videoTextures_[1]->updateImage(images[1]);
    }
  }

  //  void outputPerformanceMetrics(std::ostream& outStream) {
  //    using std::chrono::microseconds;
  //    using std::chrono::duration_cast;
  //
  //    auto now = std::chrono::high_resolution_clock::now();
  //    auto dt = now - lastTimeMesure;
  //    lastTimeMesure = now;
  //
  //    // skip first 100 frames
  //    if (skipCounter_ > 100) {
  //      float dtc = duration_cast<microseconds>(dt).count();
  //      sumFrameRate_ += 1000000.0f / dtc;
  //      outStream << sumFrameRate_ / ++numFrames_ << std::endl;
  //    } else {
  //      skipCounter_++;
  //    }
  //  }

 private:
  //
  //  int skipCounter_{0};
  //  float sumFrameRate_{0};
  //  int numFrames_{0};
  //  int drift{0};
  //  time_point lastTimeMesure{time_point::max()};

  std::vector<DynamicTextureThreadSafe*> videoTextures_;
};

#endif  // VIDEO_TEXTURE_TEXTUREUPDATECLIENT_HPP
