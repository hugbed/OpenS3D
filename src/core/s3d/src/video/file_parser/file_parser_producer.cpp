#include "s3d/video/file_parser/file_parser_producer.h"

#include "s3d/video/file_parser/ffmpeg/video_file_parser_ffmpeg.h"

#include "s3d/video/capture/video_capture_types.h"

namespace s3d {

FileParserProducer::FileParserProducer(std::string filename,
                                       s3d::ProducerConsumerMediator* mediator)
    : ProducerBarrier(mediator), videoFrame_{{}, {}}, filePath_{std::move(filename)} {}

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

  std::unique_lock<std::mutex> l(seekingMutex_);
  if (shouldSeek_) {
    shouldSeek_ = false;
    fileParser_->SeekToFrame(seekingTimestamp_);
  }

  readingFile_ = fileParser_->GetNextFrame(&videoFrame_.data_);
  videoFrame_.timestamp_ = fileParser_->CurrentFrameTimestamp();
}

const VideoFrame& FileParserProducer::getProduct() {
  return videoFrame_;
}

void FileParserProducer::seekTo(std::chrono::microseconds timestamp) {
  std::unique_lock<std::mutex> l(seekingMutex_);
  seekingTimestamp_ = timestamp;
  shouldSeek_ = true;
}

}  // namespace s3d
