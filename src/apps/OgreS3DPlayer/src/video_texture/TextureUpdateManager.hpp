//
// Created by jon on 23/03/17.
//

#ifndef OGRE_S3D_PLAYER_VIDEO_TEXTURE_TEXTURE_UPDATE_MANAGER_H
#define OGRE_S3D_PLAYER_VIDEO_TEXTURE_TEXTURE_UPDATE_MANAGER_H

#include "DynamicTextureThreadSafe.hpp"
#include "IfYUVToRGBProducer.hpp"
#include "S3DVideoRGBConsumer.hpp"
#include <condition_variable>
#include <s3d/video/video_frame.h>

#include "s3d/video/capture/video_capture_device.h"

class TextureUpdateManager {
 public:
  virtual void handleTextureUpdate(DynamicTextureThreadSafe*,
                                   DynamicTextureThreadSafe*) = 0;
};

// YUV422
class YUVFileTextureUpdateManager : public TextureUpdateManager {
 public:
  YUVFileTextureUpdateManager(const std::string& leftVideoFilename,
                              const std::string& rightVideoFilename)
      : filenames{leftVideoFilename, rightVideoFilename} {}

  // starts a thread so main thread can stop it maybe
  void handleTextureUpdate(DynamicTextureThreadSafe* videoTextureL,
                           DynamicTextureThreadSafe* videoTextureR) override {
    auto textureHandlingThread = std::thread([this, videoTextureL,
                                              videoTextureR] {
      std::mutex doneProducingMutex;
      std::condition_variable shouldConsumeCV;

      constexpr auto imageSizeBytesYUV = 1920 * 1080 * 2;
      IfYUVToRGBProducer fileProducerL(filenames.first, imageSizeBytesYUV,
                                       videoTextureL->getSizeInBytes(),
                                       &doneProducingMutex, &shouldConsumeCV);
      IfYUVToRGBProducer fileProducerR(filenames.second, imageSizeBytesYUV,
                                       videoTextureR->getSizeInBytes(),
                                       &doneProducingMutex, &shouldConsumeCV);

      std::vector<s3d::concurrency::ProducerBarrierSync<std::vector<uint8_t>>*>
          producers = {&fileProducerL, &fileProducerR};

      // start thread for each producer
      std::vector<std::thread> producerThreads;
      for (auto& producer : producers) {
        producerThreads.push_back(
            std::thread([&producer] { producer->startProducing(); }));
      }

      S3DVideoRGBConsumer consumer(videoTextureL, videoTextureR,
                                   &doneProducingMutex, &shouldConsumeCV,
                                   producers);

      // blocking
      consumer.startConsuming();

      // join all producers threads
      for (auto& producerThread : producerThreads) {
        producerThread.join();
      }

    });
    textureHandlingThread.detach();
  }

 private:
  std::pair<std::string, std::string> filenames;
};

class TextureUpdateClient : public VideoCaptureDevice::Client {
 public:
  TextureUpdateClient(DynamicTextureThreadSafe* videoTexture,
                      DynamicTextureThreadSafe* videoTextureR)
      : frameData{}, videoTexture{videoTexture}, videoTextureR{videoTextureR} {
    frameData.resize(1920 * 1080 * 3);
  }

  void OnIncomingCapturedData(const std::vector<uint8_t>& imageBytes,
                              const VideoCaptureFormat& frameFormat) override {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(3s);

    std::vector<uint8_t> testYUVData;
    testYUVData.resize(imageBytes.size());
    std::copy(std::begin(imageBytes), std::end(imageBytes), std::begin(testYUVData));
    s3d::compression::color_conversion(
        std::begin(testYUVData), std::end(testYUVData), std::begin(frameData),
        s3d::compression::YUV422{}, s3d::compression::RGB8{});
    //    videoTexture->updateImage(frameData);
    //    videoTextureR->updateImage(frameData);
  }

  void OnError(const std::string& /*unused*/) override {}
  void OnLog(const std::string& /*unused*/) override {}
  void OnStarted() override {}

 private:
  std::vector<uint8_t> frameData;
  DynamicTextureThreadSafe* videoTexture;
  DynamicTextureThreadSafe* videoTextureR;
};

#endif  // OGRE_S3D_PLAYER_VIDEO_TEXTURE_TEXTURE_UPDATE_MANAGER_H
