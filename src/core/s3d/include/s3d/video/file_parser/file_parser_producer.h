#ifndef S3D_VIDEO_FILE_PARSER_FILE_PARSER_PRODUCER_H
#define S3D_VIDEO_FILE_PARSER_FILE_PARSER_PRODUCER_H

#include "s3d/utilities/concurrency/producer_barrier.h"

#include "s3d/video/video_frame.h"

class VideoCaptureFormat;
class VideoFileParser;

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

#endif  // S3D_VIDEO_FILE_PARSER_FILE_PARSER_PRODUCER_H
