//
// Created by jon on 23/03/17.
//

#ifndef OGRE_S3D_PLAYER_FILEBYTESPROVIDER_H
#define OGRE_S3D_PLAYER_FILEBYTESPROVIDER_H

#include "SyncProducer.h"
#include "file_io.h"
#include "compression.h"

#include <fstream>

// bad class name for now
class YUVToRGBFileBytesProducer : public SyncProducer<std::vector<uint8_t>>
{
public:
    // image size, etc
    YUVToRGBFileBytesProducer(const std::string& filename,
                      std::mutex &doneProducingMutex, std::condition_variable &shouldConsumeCV)
        : SyncProducer(doneProducingMutex, shouldConsumeCV)
        , yuvBytes{}
        , rgbBytes{}
        , nbChunks{}
        , fileStream{filename, std::ios::binary}
    {
        // hardcoded for now
        yuvBytes.resize(1920 * 1080 * 2);
        rgbBytes.resize(1920 * 1080 * 3);
    }
private:
    virtual void produce() override
    {
        read_n_bytes(fileStream, yuvBytes.size(), std::begin(yuvBytes));
        yuv2rgb(std::begin(yuvBytes), std::end(yuvBytes), std::begin(rgbBytes));
        nbChunks++;
    }

    virtual const std::vector<uint8_t>& getProduct()
    {
        return rgbBytes;
    }

    virtual bool shouldStopProducing() override
    {
        return nbChunks == 100;
    }

    std::vector<uint8_t> yuvBytes;
    std::vector<uint8_t> rgbBytes;
    int nbChunks;
    std::ifstream fileStream;
};

#endif //OGRE_S3D_PLAYER_FILEBYTESPROVIDER_H
