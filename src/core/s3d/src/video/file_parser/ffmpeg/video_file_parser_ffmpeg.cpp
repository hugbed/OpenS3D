#include "s3d/video/file_parser/ffmpeg/video_file_parser_ffmpeg.h"

#include "s3d/video/file_parser/ffmpeg/decoder.h"
#include "s3d/video/file_parser/ffmpeg/scaler.h"
#include "s3d/video/file_parser/ffmpeg/seeker.h"

#include <s3d/video/capture/video_capture_types.h>

#include <ratio>

namespace s3d {

VideoFileParserFFmpeg::VideoFileParserFFmpeg(const std::string& inputFilename)
    : filename_(inputFilename), demuxer_(inputFilename) {
  decoder_ = demuxer_.createDecoder();
  seeker_ = decoder_->createSeeker();
  duration_ = decoder_->getDuration();
}

gsl::owner<VideoFileParserFFmpeg*> VideoFileParserFFmpeg::clone() const {
  return new VideoFileParserFFmpeg(filename_);
}

VideoFileParserFFmpeg::~VideoFileParserFFmpeg() = default;

bool VideoFileParserFFmpeg::Initialize(VideoCaptureFormat* format) {
  // keep suggested format if not unknown
  VideoPixelFormat suggestedPixelFormat = VideoPixelFormat::BGRA;
  if (format->pixelFormat != VideoPixelFormat::UNKNOWN) {
    suggestedPixelFormat = format->pixelFormat;
  }

  scaler_ = decoder_->createScaler(ffmpeg::pixelFormatToAV(suggestedPixelFormat));

  format->frameRate = decoder_->getFrameRate();
  format->pixelFormat = suggestedPixelFormat;
  format->frameSize = decoder_->getImageSize();
  format->stereo3D = false;

  return true;
}

void VideoFileParserFFmpeg::SeekToFrame(std::chrono::microseconds timestamp) {
  float fps = decoder_->getFrameRate();
  int frame = timestampToFrameNumber(timestamp, fps);
  int currentFrame = timestampToFrameNumber(currentTimestamp_, fps);

  int frameDelta = frame - currentFrame;
  if (frameDelta < 0 || frameDelta > 5) {
    seeker_->seekTo(timestamp);
  }

  std::chrono::microseconds frameDuration = frameNumberToTimestamp(1, fps);

  // find keypoint before timestamp
  if (frameDelta < 0) {
    auto seekTime = timestamp;
    while (GetNextFrame(nullptr) &&  // up to 3 frame precision to seek at 0
           currentTimestamp_ - timestamp > 3 * frameDuration) {
      // prevent seeking too far before 0
      auto nextSeekTime = seekTime - frameDuration;
      seekTime = nextSeekTime;
      seeker_->seekTo(seekTime);
    }
  }

  // navigate forward
  while (GetNextFrame(nullptr) &&  //
         timestamp - currentTimestamp_ > frameDuration) {
    continue;
  }
}

bool VideoFileParserFFmpeg::GetNextFrame(std::vector<uint8_t>* imageData) {
  AVFrame* frame{nullptr};

  // try to read frame from current packet
  bool frameReceived = decoder_->receiveDecodedFrame(&frame);

  // try again with new packet if no frame received
  AVPacket* packet{nullptr};
  while (!frameReceived && !decoder_->endOfFileReached()) {
    if (!demuxer_.readFrame(&packet)) {
      return false;
    }
    decoder_->sendPacketForDecoding(packet);

    // fetch new frame from sent packet
    frameReceived = decoder_->receiveDecodedFrame(&frame);
  }

  // copy received frame (converted to RGB24) to imageData vector
  if (frameReceived) {
    currentTimestamp_ = decoder_->getFrameTimeStamp(frame);
    if (imageData != nullptr) {
      scaler_->scaleFrame(frame, imageData);
    }
  }

  return !decoder_->endOfFileReached();
}

std::chrono::microseconds VideoFileParserFFmpeg::CurrentFrameTimestamp() {
  return currentTimestamp_;
}

std::chrono::microseconds VideoFileParserFFmpeg::VideoDuration() {
  return duration_;
}

// static
int VideoFileParserFFmpeg::timestampToFrameNumber(std::chrono::microseconds timestamp, float fps) {
  return static_cast<int>(fps * static_cast<float>(timestamp.count()) /
                          static_cast<float>(std::micro::den) / 10.0f);
}

// static
std::chrono::microseconds VideoFileParserFFmpeg::frameNumberToTimestamp(int frame, float fps) {
  return std::chrono::microseconds(
      static_cast<int>(static_cast<float>(frame * std::micro::den) / fps));
}

}  // namespace s3d
