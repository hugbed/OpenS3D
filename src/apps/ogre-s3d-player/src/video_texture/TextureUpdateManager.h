//
// Created by jon on 23/03/17.
//

#ifndef OGRE_S3D_PLAYER_TEXTUREUPDATEMANAGER_H
#define OGRE_S3D_PLAYER_TEXTUREUPDATEMANAGER_H

#include <condition_variable>
#include "VideoTexture.h"
#include "YUVToRGBFileBytesProducer.h"
#include "VideoBytesConsumer.h"

class TextureUpdateManager
{
public:
    virtual void handleTextureUpdate(VideoTexture*, VideoTexture*) = 0;
};

class YUVFileTextureUpdateManager : public TextureUpdateManager
{
public:
    YUVFileTextureUpdateManager(const std::string& leftVideoFilename, const std::string &rightVideoFilename)
        : filenames{leftVideoFilename, rightVideoFilename}
    {
    }

    // starts a thread so main thread can stop it maybe
    virtual void handleTextureUpdate(VideoTexture* videoTextureL, VideoTexture* videoTextureR) override
    {
        auto textureHandlingThread = std::thread([this, videoTextureL, videoTextureR]{
            std::mutex doneProducingMutex;
            std::condition_variable shouldConsumeCV;

            YUVToRGBFileBytesProducer fileProducerL(filenames.first,
                                                    doneProducingMutex, shouldConsumeCV);
            YUVToRGBFileBytesProducer fileProducerR(filenames.second,
                                                    doneProducingMutex, shouldConsumeCV);

            std::vector<SyncProducer<std::vector<uint8_t>>*> producers = {
                    &fileProducerL,
                    &fileProducerR
            };

            // start thread for each producer
            std::vector<std::thread> producerThreads;
            for (auto & producer : producers) {
                producerThreads.push_back(std::thread([&producer]{
                    producer->startProducing();
                }));
            }

            VideoBytesConsumer consumer(videoTextureL, videoTextureR,
                                        doneProducingMutex, shouldConsumeCV,
                                        producers);

            consumer.startConsuming();

            // join all producers threads
            for (auto & producerThread : producerThreads) {
                producerThread.join();
            }
        });
        textureHandlingThread.detach();
    }

private:
    std::pair<std::string, std::string> filenames;
};

#endif //OGRE_S3D_PLAYER_TEXTUREUPDATEMANAGER_H
