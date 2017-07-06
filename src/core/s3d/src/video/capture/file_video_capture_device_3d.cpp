// Copyright 2013 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#include "s3d/video/video_frame.h"
#include "s3d/video/file_parser/ffmpeg/video_file_parser_ffmpeg.h"
#include "s3d/utilities/concurrency/consumer_barrier.h"
#include "s3d/utilities/file_io.h"
#include "s3d/utilities/strings.h"
#include "s3d/video/capture/file_video_capture_device_3d.h"
#include "s3d/video/file_parser/raw_uyvy_file_parser.h"
#include "s3d/video/compression/yuv.h"

struct ProducerConsumerSynchronizer {
  ProducerConsumerSynchronizer()
      : mediatorLeft{&doneProducingMutexLeft, &shouldConsumeCV},
        mediatorRight{&doneProducingMutexRight, &shouldConsumeCV} {}

  std::mutex doneProducingMutexLeft{};
  std::mutex doneProducingMutexRight{};
  std::condition_variable shouldConsumeCV{};
  s3d::concurrency::ProducerConsumerBarrier mediatorLeft;
  s3d::concurrency::ProducerConsumerBarrier mediatorRight;
};

using s3d::concurrency::ProducerBarrier;
using s3d::concurrency::ConsumerBarrier;

class FileParserProducer : public s3d::concurrency::ProducerBarrier<VideoFrame> {
 public:
  using Base = s3d::concurrency::ProducerBarrier<VideoFrame>;

  // image size, etc
  FileParserProducer(std::string filename, s3d::concurrency::ProducerConsumerMediator* mediator);

  bool shouldStopProducing() override;

  // todo: maybe exception is more appropriate than bool? dunno
  bool allocate(VideoCaptureFormat* format);

  void produce() override;

 private:
  const VideoFrame& getProduct() override;

  bool readingFile_{false};
  std::unique_ptr<VideoFileParser> fileParser_;
  VideoFrame videoFrame_;
  std::string filePath_;
};

class FileParserConsumer : public s3d::concurrency::ConsumerBarrier<VideoFrame> {
 public:
  using Base = s3d::concurrency::ConsumerBarrier<VideoFrame>;

  FileParserConsumer(VideoCaptureDevice::Client* client,
                     VideoCaptureFormat outputFormat,
                     s3d::concurrency::ProducerConsumerMediator* mediator,
                     const Producers& producers);

  void pause();
  void resume();

  void consume() override;
  void consumeOnce();

 private:
  void sleepUntilNextFrame();

  // pause synchronization
  std::mutex pauseMutex;
  bool pauseFlag{false};
  std::condition_variable pauseCondition;

  std::chrono::duration<float> delayBetweenFrames;
  VideoCaptureFormat format_;
  VideoCaptureDevice::Client* client_;
  std::chrono::high_resolution_clock::time_point lastConsumeTime;
};

FileParserProducer::FileParserProducer(std::string filename,
                                       s3d::concurrency::ProducerConsumerMediator* mediator)
    : ProducerBarrier(mediator), filePath_{std::move(filename)}, videoFrame_{{}, {}} {}

bool FileParserProducer::shouldStopProducing() {
  return !readingFile_ || Base::shouldStopProducing();

  //    if (fileStream.eof()) {
  //      fileStream.clear();
  //      fileStream.seekg(0, std::ios::beg);
  //    }
  //    return fileStream.eof();
}

bool FileParserProducer::allocate(VideoCaptureFormat* format) {
  fileParser_ =
      std::unique_ptr<VideoFileParser>(std::make_unique<VideoFileParserFFmpeg>(filePath_));

  if (!fileParser_->Initialize(format)) {
    fileParser_.reset();
    readingFile_ = false;
  }
  videoFrame_.data_.resize(format->ImageAllocationSize());
  readingFile_ = true;
  return readingFile_;
}

void FileParserProducer::produce() {
  if (fileParser_ == nullptr) {
    readingFile_ = false;
    return;
  }
  readingFile_ = fileParser_->GetNextFrame(videoFrame_.data_);
  videoFrame_.timestamp_ = fileParser_->CurrentFrameTimestamp();
}

const VideoFrame& FileParserProducer::getProduct() {
  return videoFrame_;
}

FileParserConsumer::FileParserConsumer(VideoCaptureDevice::Client* client,
                                       VideoCaptureFormat outputFormat,
                                       s3d::concurrency::ProducerConsumerMediator* mediator,
                                       const Producers& producers)
    : ConsumerBarrier(mediator, producers),
      delayBetweenFrames(1.0f / outputFormat.frameRate),
      client_(client),
      format_{outputFormat} {}

void FileParserConsumer::consume() {
  sleepUntilNextFrame();
  consumeOnce();
}

void FileParserConsumer::consumeOnce() {
  const auto& producers = getProducers();
  auto& leftImage = producers[0]->getProduct();
  auto& rightImage = producers[1]->getProduct();
  if (client_ != nullptr) {
    client_->OnIncomingCapturedData({leftImage.data_, rightImage.data_}, format_,
                                    leftImage.timestamp_);
  }
}

void FileParserConsumer::sleepUntilNextFrame() {
  // if should pause, wait
  {
    std::unique_lock<std::mutex> lk(pauseMutex);
    while (pauseFlag) {
      pauseCondition.wait(lk, [this] { return !pauseFlag; });
    }
  }

  // todo: this is not precise, do as timed loop sleep
  // with a bit of busy waiting and using next consume time instead of sleep = now + delta
  auto t2 = std::chrono::high_resolution_clock::now();
  auto sleepTime = delayBetweenFrames -
                   std::chrono::duration_cast<std::chrono::microseconds>(t2 - lastConsumeTime);
  std::this_thread::sleep_for(sleepTime);
  lastConsumeTime = std::chrono::high_resolution_clock::now();
}

void FileParserConsumer::pause() {
  std::unique_lock<std::mutex> lock;
  pauseFlag = true;
}

void FileParserConsumer::resume() {
  {
    std::unique_lock<std::mutex> lock;
    pauseFlag = false;
  }
  pauseCondition.notify_all();
}

FileVideoCaptureDevice3D::FileVideoCaptureDevice3D(const std::string& filePathsStr) {
  std::vector<std::string> filePaths;
  s3d::split(filePathsStr, ';', std::back_inserter(filePaths));
  if (filePaths.size() == 2) {
    std::cout << "First:" << filePaths[0] << std::endl;
    std::cout << "Second:" << filePaths[1] << std::endl;
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

FileVideoCaptureDevice3D::~FileVideoCaptureDevice3D() {
  WaitUntilDone();
}

void FileVideoCaptureDevice3D::AllocateAndStart(const VideoCaptureFormat& format, Client* client) {
  client_ = client;
  captureFormat_ = format;

  Allocate();
  Start();
}

// todo: unit test that each mediator has a different mutex, good luck
void FileVideoCaptureDevice3D::Allocate() {
  sync_ = std::make_unique<ProducerConsumerSynchronizer>();

  // todo: this should be taken from format parameter and validated by file parser
  producers_.first = std::make_unique<FileParserProducer>(filePaths_.first, &sync_->mediatorLeft);
  producers_.second =
      std::make_unique<FileParserProducer>(filePaths_.second, &sync_->mediatorRight);

  if (!producers_.first->allocate(&captureFormat_) ||
      !producers_.second->allocate(&captureFormat_)) {
    throw VideoCaptureDeviceAllocationException(
        "Cannot open requested file(s)");  // todo: write the name of the files here
  }

  std::vector<FileParserProducer::Base*> producers = {producers_.first.get(),
                                                      producers_.second.get()};

  captureFormat_.stereo3D = true;
  consumer_ = std::make_unique<FileParserConsumer>(client_, captureFormat_, &sync_->mediatorLeft,
                                                   producers);
}

void FileVideoCaptureDevice3D::Start() {
  captureThread_ = std::make_unique<std::thread>([this] {
    std::vector<FileParserProducer::Base*> producers = {producers_.first.get(),
                                                        producers_.second.get()};
    // start thread for each producer
    std::vector<std::thread> producerThreads;
    for (auto& producer : producers) {
      producerThreads.emplace_back([&producer] { producer->startProducing(); });
    }

    // todo: maybe a deadlock if producers are not ready to produce yet
    // blocking
    consumer_->startConsuming();

    // join all producers threads
    for (auto& producerThread : producerThreads) {
      producerThread.join();
    }
  });
}

void FileVideoCaptureDevice3D::WaitUntilDone() {
  if (captureThread_ != nullptr) {
    captureThread_->join();
    captureThread_.reset();
  }
}

void FileVideoCaptureDevice3D::StopAndDeAllocate() {
  // this stops the producers
  consumer_->resume();  // wake it up so it can stop
  consumer_->stop();
  WaitUntilDone();
}

void FileVideoCaptureDevice3D::RequestRefreshFrame() {
  VideoCaptureDevice::RequestRefreshFrame();
  // manual production and consumption
  producers_.first->produce();
  producers_.second->produce();
  consumer_->consumeOnce();
}

VideoCaptureFormat FileVideoCaptureDevice3D::DefaultFormat() {
  VideoCaptureFormat format{};
  VideoFileParserFFmpeg p(filePaths_.first);
  p.Initialize(&format);
  format.stereo3D = true;
  return format;
}

void FileVideoCaptureDevice3D::MaybeSuspend() {
  consumer_->pause();
}

void FileVideoCaptureDevice3D::Resume() {
  consumer_->resume();
}
