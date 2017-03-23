//
// Created by jon on 23/03/17.
//

#ifndef OGRE_S3D_PLAYER_VIDEOCONSUMER_H
#define OGRE_S3D_PLAYER_VIDEOCONSUMER_H

#include "SyncConsumer.h"
#include "VideoTexture.h"

class VideoConsumer : public SyncConsumer<std::vector<uint8_t>>
{
public:
    VideoConsumer(VideoTexture* videoTextureL, VideoTexture *videoTextureR,
        std::mutex &doneProducingMutex,
        std::condition_variable &shouldConsumeCV,
        const std::vector<SyncProducer<std::vector<uint8_t>>*> producers)
    : SyncConsumer(doneProducingMutex, shouldConsumeCV, producers)
    , videoTextureL{videoTextureL}
    , videoTextureR{videoTextureR}
    {
    }
private:
    virtual void consume() override
    {
        sleepUntilNextFrame();
        auto& leftImage = producers[0]->getProduct();
        auto& rightImage = producers[1]->getProduct();
        videoTextureL->pushFrame(leftImage);
        videoTextureR->pushFrame(rightImage);
        nbFrames++;
    }

    void sleepUntilNextFrame()
    {
        using namespace std::chrono_literals;
        auto t2 = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(33.33ms - std::chrono::duration_cast<std::chrono::milliseconds>(t2-lastConsumeTime));
        lastConsumeTime = std::chrono::high_resolution_clock::now();
    }

    virtual bool shouldStopConsuming() override
    {
        return nbFrames == 100;
    }

    int nbFrames{0};
    VideoTexture* videoTextureL;
    VideoTexture* videoTextureR;
    std::chrono::high_resolution_clock::time_point lastConsumeTime;
};

#endif //OGRE_S3D_PLAYER_VIDEOCONSUMER_H
