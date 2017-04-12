//
// Created by jon on 11/04/17.
//

#include <s3d/video/capture/video_file_parser.h>
#include "s3d/utilities/concurrency/consumer_barrier_sync.h"
#include "s3d/utilities/file_io.h"
#include "s3d/utilities/strings_util.h"
#include "s3d/video/capture/file_video_capture_device_3d.h"
#include "s3d/video/capture/video_file_parser.h"
#include "s3d/video/compression/yuv.h"

// todo(hugbed): should use RawUYVYFileParser

using s3d::concurrency::ProducerBarrierSync;
using s3d::concurrency::ConsumerBarrierSync;

class RawUYVY3DFileParserProducer
    : public s3d::concurrency::ProducerBarrierSync<std::vector<uint8_t>> {
 public:
  using ProducerType =
      s3d::concurrency::ProducerBarrierSync<std::vector<uint8_t>>;

  // image size, etc
  RawUYVY3DFileParserProducer(std::string filename,
                              VideoCaptureFormat imageFileFormat,
                              std::mutex* doneProducingMutex,
                              std::condition_variable* shouldConsumeCV)
      : ProducerBarrierSync(doneProducingMutex, shouldConsumeCV),
        filePath_{std::move(filename)} {
    imageFileFormat.pixelFormat = VideoPixelFormat::RGB;
    rgbBytes.resize(imageFileFormat.ImageAllocationSize());
  }

  bool shouldStopProducing() override {
    // read in loop :D

    return !readingFile_;

    //    if (fileStream.eof()) {
    //      fileStream.clear();
    //      fileStream.seekg(0, std::ios::beg);
    //    }
    //    return fileStream.eof();
  }

 private:
  void produce() override {
    readingFile_ = fileParser_->GetNextFrame(rgbBytes);
  }

  void onStartProducing() override {
    fileParser_ = std::unique_ptr<VideoFileParser>(
        std::make_unique<RawUYVYFileParser>(filePath_));

    VideoCaptureFormat fileFormat;
    if (!fileParser_->Initialize(&fileFormat)) {
      fileParser_.reset();
      return;
    }

    readingFile_ = true;
  }

  const std::vector<uint8_t>& getProduct() override { return rgbBytes; }

  bool readingFile_{true};
  std::unique_ptr<VideoFileParser> fileParser_;
  std::vector<uint8_t> rgbBytes;
  std::string filePath_;
};

class RawUYVY3DFileParserConsumer
    : public s3d::concurrency::ConsumerBarrierSync<std::vector<uint8_t>> {
 public:
  RawUYVY3DFileParserConsumer(VideoCaptureDevice3D::Client* client,
                              VideoCaptureFormat outputFormat,
                              std::mutex* doneProducingMutex,
                              std::condition_variable* shouldConsumeCV,
                              const Producers producers)
      : ConsumerBarrierSync(doneProducingMutex, shouldConsumeCV, producers),
        format_(outputFormat),
        delayBetweenFrames(1.0f / outputFormat.frameRate),
        client_(client) {}

 private:
  void consume() override {
    sleepUntilNextFrame();
    const auto& producers = getProducers();
    auto& leftImage = producers[0]->getProduct();
    auto& rightImage = producers[1]->getProduct();
    client_->OnIncomingCapturedData(leftImage, rightImage, format_);
  }

  void sleepUntilNextFrame() {
    auto t2 = std::chrono::high_resolution_clock::now();
    auto sleepTime = delayBetweenFrames -
                     std::chrono::duration_cast<std::chrono::microseconds>(
                         t2 - lastConsumeTime);
    std::this_thread::sleep_for(sleepTime);
    lastConsumeTime = std::chrono::high_resolution_clock::now();
  }

  std::chrono::duration<float> delayBetweenFrames;
  VideoCaptureFormat format_;
  VideoCaptureDevice3D::Client* client_;
  std::chrono::high_resolution_clock::time_point lastConsumeTime;
};

FileVideoCaptureDevice3D::FileVideoCaptureDevice3D(std::string filePathsStr)
    : stopCaptureFlag_(false) {
  std::vector<std::string> filePaths;
  s3d::split(filePathsStr, ';', std::back_inserter(filePaths));
  if (filePaths.size() == 2) {
    filePaths_.first = std::move(filePaths[0]);
    filePaths_.second = std::move(filePaths[1]);
  } else {
    // todo: oh oh
  }
}

FileVideoCaptureDevice3D::~FileVideoCaptureDevice3D() {
  // check that thread is not still running
}

void FileVideoCaptureDevice3D::AllocateAndStart(
    const VideoCaptureFormat& /*format*/,
    std::unique_ptr<Client> client) {
  client_ = std::move(client);
  auto captureThread = std::thread([this] {
    std::mutex doneProducingMutex;
    std::condition_variable shouldConsumeCV;

    VideoCaptureFormat fileFormat(Size(1920, 1080), 30.0f,
                                  VideoPixelFormat::UNKNOWN);
    producers_.first = std::make_unique<RawUYVY3DFileParserProducer>(
        filePaths_.first, fileFormat, &doneProducingMutex, &shouldConsumeCV);
    producers_.second = std::make_unique<RawUYVY3DFileParserProducer>(
        filePaths_.second, fileFormat, &doneProducingMutex, &shouldConsumeCV);

    std::vector<RawUYVY3DFileParserProducer::ProducerType*> producers = {
        producers_.first.get(), producers_.second.get()};

    // start thread for each producer
    std::vector<std::thread> producerThreads;
    for (auto& producer : producers) {
      producerThreads.push_back(
          std::thread([&producer] { producer->startProducing(); }));
    }

    VideoCaptureFormat outputFormat(fileFormat.frameSize, fileFormat.frameRate,
                                    VideoPixelFormat::RGB);
    consumer_ = std::make_unique<RawUYVY3DFileParserConsumer>(
        client_.get(), outputFormat, &doneProducingMutex, &shouldConsumeCV,
        producers);

    // blocking
    consumer_->startConsuming();

    // join all producers threads
    for (auto& producerThread : producerThreads) {
      producerThread.join();
    }
  });

  captureThread.detach();
}

void FileVideoCaptureDevice3D::StopAndDeAllocate() {
  // call producers/consumers with some flag
}