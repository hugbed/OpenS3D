#include "s3d/video/file_parser/ffmpeg/video_file_parser_ffmpeg.h"

#include "s3d/video/file_parser/ffmpeg/decoder.h"
#include "s3d/video/file_parser/ffmpeg/scaler.h"
#include "s3d/video/file_parser/ffmpeg/seeker.h"

#include <s3d/video/capture/video_capture_types.h>

#include <ratio>

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

  int frame = static_cast<int>(fps * static_cast<float>(timestamp.count()) /
                               static_cast<float>(std::micro::den) / 10.0f);
  int currentFrame = static_cast<int>(fps * static_cast<float>(currentTimestamp_.count()) /
                                      static_cast<float>(std::micro::den) / 10.0f);
  int frameDelta = frame - currentFrame;

  auto seekTime = timestamp;

  if (frameDelta < 0 || frameDelta > 5) {
    seeker_->seekTo(seekTime);
  }

  float eps = static_cast<float>(std::micro::den) / fps;
  int64_t check = timestamp.count() - currentTimestamp_.count();

  // find keypoint before timestamp
  if (frameDelta < 0) {
    while (GetNextFrame(nullptr) &&
           currentTimestamp_.count() - timestamp.count() >
               static_cast<float>(std::micro::den) / fps) {
      using std::chrono_literals::operator""ms;
      seekTime -=
          std::chrono::microseconds(static_cast<int>(static_cast<float>(std::micro::den) / fps));
      seeker_->seekTo(seekTime);
    }
  }

  // navigate forward
  while (GetNextFrame(nullptr) &&
         timestamp.count() - currentTimestamp_.count() >
             static_cast<float>(std::micro::den) / fps) {
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
