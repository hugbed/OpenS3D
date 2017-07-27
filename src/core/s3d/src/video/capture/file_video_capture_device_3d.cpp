// Copyright 2013 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#include "s3d/video/capture/file_video_capture_device_3d.h"

#include "s3d/video/file_parser/ffmpeg/video_file_parser_ffmpeg.h"
#include "s3d/utilities/file_io.h"
#include "s3d/utilities/strings.h"
#include "s3d/video/compression/yuv.h"

#include "s3d/video/file_parser/file_parser_consumer.h"
#include "s3d/video/file_parser/file_parser_producer.h"

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
  producers_.first = std::make_unique<FileParserProducer>(filePaths_.first, sync_->mediator1.get());
  producers_.second =
      std::make_unique<FileParserProducer>(filePaths_.second, sync_->mediator2.get());

  if (!producers_.first->allocate(&captureFormat_) ||
      !producers_.second->allocate(&captureFormat_)) {
    throw VideoCaptureDeviceAllocationException(
        "Cannot open requested file(s)");  // todo: write the name of the files here
  }

  std::vector<FileParserProducer::Base*> producers = {producers_.first.get(),
                                                      producers_.second.get()};

  captureFormat_.stereo3D = true;
  consumer_ =
      std::make_unique<FileParserConsumer>(client_, captureFormat_,
                                           std::vector<s3d::concurrency::ProducerConsumerMediator*>{
                                               sync_->mediator1.get(), sync_->mediator2.get()},
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

void FileVideoCaptureDevice3D::MaybeSeekTo(std::chrono::microseconds timestamp) {
  VideoCaptureDevice::MaybeSeekTo(timestamp);
  producers_.first->seekTo(timestamp);
  producers_.second->seekTo(timestamp);
  RequestRefreshFrame();
}
