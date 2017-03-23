//
// Created by jon on 23/03/17.
//

#ifndef OGRE_S3D_PLAYER_FILEBYTESPROVIDER_H
#define OGRE_S3D_PLAYER_FILEBYTESPROVIDER_H

#include "../utils/ProducerBarrierSync.h"
#include "../utils/file_io.h"
#include "../utils/compression.h"

#include <fstream>

class IfYUVToRGBProducer : public ProducerBarrierSync<std::vector<uint8_t>>
{
public:
    // image size, etc
    IfYUVToRGBProducer(const std::string& filename, size_t imageSizeBytesYUV, size_t imageSizeBytesRGB,
                      std::mutex &doneProducingMutex, std::condition_variable &shouldConsumeCV)
        : ProducerBarrierSync(doneProducingMutex, shouldConsumeCV)
        , yuvBytes{}
        , rgbBytes{}
        , fileStream{filename, std::ios::binary}
    {
        // hardcoded for now
        yuvBytes.resize(imageSizeBytesYUV);
        rgbBytes.resize(imageSizeBytesRGB);
    }

    virtual bool shouldStopProducing() override
    {
        // read in loop :D
        if (fileStream.eof()) {
            fileStream.clear();
            fileStream.seekg(0, std::ios::beg);
        }
        return fileStream.eof();
    }

private:

    virtual void produce() override
    {
        read_n_bytes(fileStream, yuvBytes.size(), std::begin(yuvBytes));
        yuv2rgb(std::begin(yuvBytes), std::end(yuvBytes), std::begin(rgbBytes));
    }

    virtual const std::vector<uint8_t>& getProduct()
    {
        return rgbBytes;
    }

    std::vector<uint8_t> yuvBytes;
    std::vector<uint8_t> rgbBytes;
    std::ifstream fileStream;
};

#endif //OGRE_S3D_PLAYER_FILEBYTESPROVIDER_H
