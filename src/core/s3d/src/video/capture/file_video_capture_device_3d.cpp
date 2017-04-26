// Copyright 2013 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#include "s3d/utilities/concurrency/consumer_barrier.h"
#include "s3d/utilities/file_io.h"
#include "s3d/utilities/strings.h"
#include "s3d/video/capture/file_video_capture_device_3d.h"
#include "s3d/video/capture/video_file_parser.h"
#include "s3d/video/compression/yuv.h"

// todo(hugbed): should use RawUYVYFileParser

using s3d::concurrency::ProducerBarrier;
using s3d::concurrency::ConsumerBarrier;

class RawUYVY3DFileParserProducer : public s3d::concurrency::ProducerBarrier<std::vector<uint8_t>> {
 public:
  using ProducerType = s3d::concurrency::ProducerBarrier<std::vector<uint8_t>>;

  // image size, etc
  RawUYVY3DFileParserProducer(std::string filename,
                              VideoCaptureFormat imageFileFormat,
                              s3d::concurrency::ProducerConsumerMediator* mediator);

  bool shouldStopProducing() override;

  // todo: maybe exception is more appropriate than bool? dunno
  bool allocate();

 private:
  void produce() override;
  const std::vector<uint8_t>& getProduct() override;

  bool readingFile_{false};
  std::unique_ptr<VideoFileParser> fileParser_;
  std::vector<uint8_t> rgbBytes;
  std::string filePath_;
};

class RawUYVY3DFileParserConsumer : public s3d::concurrency::ConsumerBarrier<std::vector<uint8_t>> {
 public:
  RawUYVY3DFileParserConsumer(VideoCaptureDevice::Client* client,
                              VideoCaptureFormat outputFormat,
                              s3d::concurrency::ProducerConsumerMediator* mediator,
                              const Producers& producers);

 private:
  void consume() override;
  void sleepUntilNextFrame();

  std::chrono::duration<float> delayBetweenFrames;
  VideoCaptureFormat format_;
  VideoCaptureDevice::Client* client_;
  std::chrono::high_resolution_clock::time_point lastConsumeTime;
};

RawUYVY3DFileParserProducer::RawUYVY3DFileParserProducer(
    std::string filename,
    VideoCaptureFormat imageFileFormat,
    s3d::concurrency::ProducerConsumerMediator* mediator)
    : ProducerBarrier(mediator), filePath_{std::move(filename)} {
  imageFileFormat.pixelFormat = VideoPixelFormat::RGB;
  rgbBytes.resize(imageFileFormat.ImageAllocationSize());
}

bool RawUYVY3DFileParserProducer::shouldStopProducing() {
  return !readingFile_;

  //    if (fileStream.eof()) {
  //      fileStream.clear();
  //      fileStream.seekg(0, std::ios::beg);
  //    }
  //    return fileStream.eof();
}

bool RawUYVY3DFileParserProducer::allocate() {
  fileParser_ = std::unique_ptr<VideoFileParser>(std::make_unique<RawUYVYFileParser>(filePath_));

  VideoCaptureFormat fileFormat;
  if (!fileParser_->Initialize(&fileFormat)) {
    fileParser_.reset();
    readingFile_ = false;
  }
  readingFile_ = true;
  return readingFile_;
}

void RawUYVY3DFileParserProducer::produce() {
  if (fileParser_ == nullptr) {
    readingFile_ = false;
    return;
  }

  readingFile_ = fileParser_->GetNextFrame(rgbBytes);
}

const std::vector<uint8_t>& RawUYVY3DFileParserProducer::getProduct() {
  return rgbBytes;
}

RawUYVY3DFileParserConsumer::RawUYVY3DFileParserConsumer(
    VideoCaptureDevice::Client* client,
    VideoCaptureFormat outputFormat,
    s3d::concurrency::ProducerConsumerMediator* mediator,
    const Producers& producers)
    : ConsumerBarrier(mediator, producers),
      format_(outputFormat),
      delayBetweenFrames(1.0f / outputFormat.frameRate),
      client_(client) {}

void RawUYVY3DFileParserConsumer::consume() {
  sleepUntilNextFrame();
  const auto& producers = getProducers();
  auto& leftImage = producers[0]->getProduct();
  auto& rightImage = producers[1]->getProduct();
  if (client_ != nullptr) {
    client_->OnIncomingCapturedData({leftImage, rightImage}, format_);
  }
}

void RawUYVY3DFileParserConsumer::sleepUntilNextFrame() {
  auto t2 = std::chrono::high_resolution_clock::now();
  auto sleepTime = delayBetweenFrames -
                   std::chrono::duration_cast<std::chrono::microseconds>(t2 - lastConsumeTime);
  std::this_thread::sleep_for(sleepTime);
  lastConsumeTime = std::chrono::high_resolution_clock::now();
}

FileVideoCaptureDevice3D::FileVideoCaptureDevice3D(const std::string& filePathsStr)
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

gsl::owner<VideoCaptureDevice*> FileVideoCaptureDevice3D::clone() const {
  auto& combinedPath = std::string(filePaths_.first).append(filePaths_.second);
  return new FileVideoCaptureDevice3D(combinedPath);
}

FileVideoCaptureDevice3D::~FileVideoCaptureDevice3D() = default;

void FileVideoCaptureDevice3D::AllocateAndStart(const VideoCaptureFormat& format,
                                                std::unique_ptr<Client> client) {
  client_ = std::move(client);

  auto captureThread = std::thread([this, format] {
    std::mutex doneProducingMutex;
    std::condition_variable shouldConsumeCV;

    s3d::concurrency::ProducerConsumerBarrier mediator(&doneProducingMutex, &shouldConsumeCV);

    VideoCaptureFormat fileFormat(Size(1920, 1080), 30.0f, VideoPixelFormat::UNKNOWN);
    producers_.first =
        std::make_unique<RawUYVY3DFileParserProducer>(filePaths_.first, fileFormat, &mediator);
    producers_.second =
        std::make_unique<RawUYVY3DFileParserProducer>(filePaths_.second, fileFormat, &mediator);

    if (!producers_.first->allocate() || !producers_.second->allocate()) {
      client_->OnError("Cannot open requested file(s)");
    }

    std::vector<RawUYVY3DFileParserProducer::ProducerType*> producers = {producers_.first.get(),
                                                                         producers_.second.get()};

    // start thread for each producer
    std::vector<std::thread> producerThreads;
    for (auto& producer : producers) {
      producerThreads.emplace_back([&producer] { producer->startProducing(); });
    }

    consumer_ =
        std::make_unique<RawUYVY3DFileParserConsumer>(client_.get(), format, &mediator, producers);

    client_->OnStarted();

    // todo: maybe a deadlock if producers are not ready to produce yet
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
