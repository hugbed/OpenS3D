//
// Created by jon on 23/03/17.
//

#ifndef OGRE_S3D_PLAYER_TEXTUREUPDATEMANAGER_H
#define OGRE_S3D_PLAYER_TEXTUREUPDATEMANAGER_H

#include <condition_variable>
#include "DynamicTextureThreadSafe.h"
#include "IfYUVToRGBProducer.h"
#include "S3DVideoRGBConsumer.h"

#include "s3d/video/capture/video_capture_device.h"

class TextureUpdateManager
{
public:
    virtual void handleTextureUpdate(DynamicTextureThreadSafe*, DynamicTextureThreadSafe*) = 0;
};

// YUV422
class YUVFileTextureUpdateManager : public TextureUpdateManager
{
public:
    YUVFileTextureUpdateManager(const std::string& leftVideoFilename, const std::string &rightVideoFilename)
        : filenames{leftVideoFilename, rightVideoFilename}
    {
    }

    // starts a thread so main thread can stop it maybe
    virtual void handleTextureUpdate(DynamicTextureThreadSafe* videoTextureL, DynamicTextureThreadSafe* videoTextureR) override
    {
        auto textureHandlingThread = std::thread([this, videoTextureL, videoTextureR]{
            std::mutex doneProducingMutex;
            std::condition_variable shouldConsumeCV;

            constexpr auto imageSizeBytesYUV = 1920 * 1080 * 2;
            IfYUVToRGBProducer fileProducerL(filenames.first, imageSizeBytesYUV, videoTextureL->getSizeInBytes(),
                                             doneProducingMutex, shouldConsumeCV);
            IfYUVToRGBProducer fileProducerR(filenames.second, imageSizeBytesYUV, videoTextureR->getSizeInBytes(),
                                             doneProducingMutex, shouldConsumeCV);

            std::vector<s3d::concurrency::ProducerBarrierSync<std::vector<uint8_t>>*> producers = {
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

            S3DVideoRGBConsumer consumer(videoTextureL, videoTextureR,
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

class TextureUpdateClient : public VideoCaptureDevice::Client {
public:
    TextureUpdateClient(DynamicTextureThreadSafe* videoTexture, DynamicTextureThreadSafe* videoTextureR)
        : videoTexture{videoTexture}
        , videoTextureR{videoTextureR}
    {
        frameData.resize(1920*1080*3);
    }

    virtual void OnIncomingCapturedData(const std::vector<uint8_t> &data,
                                        const VideoCaptureFormat& frameFormat) {
        s3d::compression::color_conversion(std::begin(data), std::end(data), std::begin(frameData),
                                           s3d::compression::YUV422{}, s3d::compression::RGB8{});
        videoTexture->updateImage(frameData);
        videoTextureR->updateImage(frameData);
    }

    virtual void OnError(const std::string& reason) override {}
    virtual void OnLog(const std::string& message) override {}
    virtual void OnStarted() override {}

private:
    std::vector<uint8_t> frameData;
    DynamicTextureThreadSafe* videoTexture;
    DynamicTextureThreadSafe* videoTextureR;
};

#endif //OGRE_S3D_PLAYER_TEXTUREUPDATEMANAGER_H
