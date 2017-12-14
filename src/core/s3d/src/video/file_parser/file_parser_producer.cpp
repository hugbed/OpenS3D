#include "s3d/video/file_parser/file_parser_producer.h"

#include "s3d/video/capture/video_capture_types.h"
#include "s3d/video/file_parser/video_file_parser.h"

namespace s3d {

FileParserProducer::FileParserProducer(std::string filename,
                                       s3d::ProducerConsumerMediator* mediator)
    : ProducerBarrier(mediator), videoFrame_{{}, {}} {}

bool FileParserProducer::shouldStopProducing() {
  return !readingFile_ || Base::shouldStopProducing();

  //    if (fileStream.eof()) {
  //      fileStream.clear();
  //      fileStream.seekg(0, std::ios::beg);
  //    }
  //    return fileStream.eof();
}

bool FileParserProducer::allocate(VideoCaptureFormat* format, std::unique_ptr<VideoFileParser> fileParser) {
  fileParser_ = std::move(fileParser);

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
