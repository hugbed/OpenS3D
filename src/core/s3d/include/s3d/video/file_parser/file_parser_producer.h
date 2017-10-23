#ifndef S3D_VIDEO_FILE_PARSER_FILE_PARSER_PRODUCER_H
#define S3D_VIDEO_FILE_PARSER_FILE_PARSER_PRODUCER_H

#include "s3d/concurrency/producer_barrier.h"

#include "s3d/video/video_frame.h"

#include <atomic>
#include <chrono>
#include <mutex>

namespace s3d {

struct VideoCaptureFormat;
class VideoFileParser;

using s3d::ProducerBarrier;

struct ProducerConsumerSynchronizer {
  ProducerConsumerSynchronizer() {
    mediator1 = std::make_unique<s3d::ProducerConsumerBarrier>(&consumerLatch, &producer1Semaphore);
    mediator2 = std::make_unique<s3d::ProducerConsumerBarrier>(&consumerLatch, &producer2Semaphore);
  }

  s3d::BinarySemaphore producer1Semaphore{};
  s3d::BinarySemaphore producer2Semaphore{};
  s3d::CyclicCountDownLatch consumerLatch{2};

  std::unique_ptr<s3d::ProducerConsumerBarrier> mediator1;
  std::unique_ptr<s3d::ProducerConsumerBarrier> mediator2;
};

class FileParserProducer : public s3d::ProducerBarrier<VideoFrame> {
 public:
  using Base = s3d::ProducerBarrier<VideoFrame>;

  // image size, etc
  FileParserProducer(std::string filename, s3d::ProducerConsumerMediator* mediator);

  bool shouldStopProducing() override;

  // todo: maybe exception is more appropriate than bool? dunno
  bool allocate(VideoCaptureFormat* format);

  void produce() override;

  void seekTo(std::chrono::microseconds timestamp);

 private:
  const VideoFrame& getProduct() override;

  // seeking
  std::atomic<bool> shouldSeek_{false};
  std::chrono::microseconds seekingTimestamp_;
  std::mutex seekingMutex_;

  bool readingFile_{false};
  std::unique_ptr<VideoFileParser> fileParser_;
  VideoFrame videoFrame_;
  std::string filePath_;
};

}  // namespace s3d

#endif  // S3D_VIDEO_FILE_PARSER_FILE_PARSER_PRODUCER_H
