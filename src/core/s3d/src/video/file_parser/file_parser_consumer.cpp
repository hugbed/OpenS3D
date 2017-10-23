#include "s3d/video/file_parser/file_parser_consumer.h"

namespace s3d {

using Base = FileParserConsumer::Base;

FileParserConsumer::FileParserConsumer(VideoCaptureDevice::Client* client,
                                       VideoCaptureFormat outputFormat,
                                       Base::Mediators mediators,
                                       const Producers& producers)
    : ConsumerBarrier(mediators, producers),
      delayBetweenFrames(1.0f / outputFormat.frameRate),
      format_{outputFormat},
      client_(client) {}

void FileParserConsumer::consume() {
  sleepUntilNextFrame();
  consumeOnce();
}

void FileParserConsumer::consumeOnce() {
  const auto& producers = getProducers();
  auto& leftImage = producers[0]->getProduct();
  auto& rightImage = producers[1]->getProduct();
  if (client_ != nullptr) {
    client_->OnIncomingCapturedData(
        {leftImage.data_, rightImage.data_}, format_, leftImage.timestamp_);
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
  // with a bit of busy waiting and using next consume time instead of sleep =
  // now + delta
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

}  // namespace s3d
