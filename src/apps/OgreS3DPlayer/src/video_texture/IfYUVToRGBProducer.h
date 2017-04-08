//
// Created by jon on 23/03/17.
//

#ifndef OGRE_S3D_PLAYER_VIDEO_TEXTURE_IF_YUV_TO_RGB_PRODUCER_H
#define OGRE_S3D_PLAYER_VIDEO_TEXTURE_IF_YUV_TO_RGB_PRODUCER_H

#include "s3d/utilities/concurrency/producer_barrier_sync.h"
#include "s3d/utilities/file_io.h"
#include "s3d/utilities/yuv.h"

#include <fstream>

class FileNotFoundException {};

class IfYUVToRGBProducer
    : public s3d::concurrency::ProducerBarrierSync<std::vector<uint8_t>> {
 public:
  // image size, etc
  IfYUVToRGBProducer(const std::string& filename,
                     size_t imageSizeBytesYUV,
                     size_t imageSizeBytesRGB,
                     std::mutex& doneProducingMutex,
                     std::condition_variable& shouldConsumeCV)
      : ProducerBarrierSync(doneProducingMutex, shouldConsumeCV),
        yuvBytes{},
        rgbBytes{},
        fileStream{filename, std::ios::binary} {
    if (!fileStream.is_open())
      throw FileNotFoundException{};  // todo no no, no exception here please
    // hardcoded for now
    yuvBytes.resize(imageSizeBytesYUV);
    rgbBytes.resize(imageSizeBytesRGB);
  }

  virtual bool shouldStopProducing() override {
    // read in loop :D
    if (fileStream.eof()) {
      fileStream.clear();
      fileStream.seekg(0, std::ios::beg);
    }
    return fileStream.eof();
  }

 private:
  virtual void produce() override {
    using namespace s3d::file_io;
    using namespace s3d::compression;

    read_n_bytes(fileStream, yuvBytes.size(), std::begin(yuvBytes));
    color_conversion(std::begin(yuvBytes), std::end(yuvBytes),
                     std::begin(rgbBytes), YUV422{}, RGB8{});
  }

  virtual const std::vector<uint8_t>& getProduct() { return rgbBytes; }

  std::vector<uint8_t> yuvBytes;
  std::vector<uint8_t> rgbBytes;
  std::ifstream fileStream;
};

#endif  // OGRE_S3D_PLAYER_VIDEO_TEXTURE_IF_YUV_TO_RGB_PRODUCER_H
