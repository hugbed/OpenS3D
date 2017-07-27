#ifndef S3D_VIDEO_FILE_PARSER_FILE_PARSER_CONSUMER_H
#define S3D_VIDEO_FILE_PARSER_FILE_PARSER_CONSUMER_H

#include "s3d/concurrency/consumer_barrier.h"

#include "s3d/video/capture/video_capture_device.h"

#include "s3d/video/video_frame.h"

class FileParserConsumer : public s3d::ConsumerBarrier<VideoFrame> {
 public:
  using Base = s3d::ConsumerBarrier<VideoFrame>;

  FileParserConsumer(VideoCaptureDevice::Client* client,
                     VideoCaptureFormat outputFormat,
                     Base::Mediators mediators,
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

#endif  // S3D_VIDEO_FILE_PARSER_FILE_PARSER_CONSUMER_H
