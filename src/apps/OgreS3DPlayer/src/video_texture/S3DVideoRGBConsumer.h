//
// Created by jon on 23/03/17.
//

#ifndef OGRE_S3D_PLAYER_VIDEO_TEXTURE_S3D_VIDEO_RGB_CONSUMER_H
#define OGRE_S3D_PLAYER_VIDEO_TEXTURE_S3D_VIDEO_RGB_CONSUMER_H

#include "DynamicTextureThreadSafe.h"
#include "s3d/utilities/concurrency/consumer_barrier_sync.h"

class S3DVideoRGBConsumer
    : public s3d::concurrency::ConsumerBarrierSync<std::vector<uint8_t>> {
 public:
  S3DVideoRGBConsumer(DynamicTextureThreadSafe* videoTextureL,
                      DynamicTextureThreadSafe* videoTextureR,
                      std::mutex& doneProducingMutex,
                      std::condition_variable& shouldConsumeCV,
                      const std::vector<s3d::concurrency::ProducerBarrierSync<
                          std::vector<uint8_t>>*> producers)
      : ConsumerBarrierSync(doneProducingMutex, shouldConsumeCV, producers),
        videoTextureL{videoTextureL},
        videoTextureR{videoTextureR} {}

 private:
  virtual void consume() override {
    sleepUntilNextFrame();
    const auto& producers = getProducers();
    auto& leftImage = producers[0]->getProduct();
    auto& rightImage = producers[1]->getProduct();
    videoTextureL->updateImage(leftImage);
    videoTextureR->updateImage(rightImage);
    nbFrames++;
  }

  void sleepUntilNextFrame() {
    auto t2 = std::chrono::high_resolution_clock::now();
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(
        33.33ms - std::chrono::duration_cast<std::chrono::milliseconds>(
                      t2 - lastConsumeTime));
    lastConsumeTime = std::chrono::high_resolution_clock::now();
  }

  int nbFrames{0};
  DynamicTextureThreadSafe* videoTextureL;
  DynamicTextureThreadSafe* videoTextureR;
  std::chrono::high_resolution_clock::time_point lastConsumeTime;
};

#endif  // OGRE_S3D_PLAYER_VIDEO_TEXTURE_S3D_VIDEO_RGB_CONSUMER_H
