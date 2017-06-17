#include "VideoFileParserFFmpeg.h"

#include "Decoder.h"
#include "Scaler.h"

VideoFileParserFFmpeg::VideoFileParserFFmpeg(const std::string& inputFilename)
    : demuxer_(inputFilename) {
  decoder_ = demuxer_.createDecoder();
  scaler_ = decoder_->createScaler(AV_PIX_FMT_RGB24);
}

VideoFileParserFFmpeg::~VideoFileParserFFmpeg() = default;

bool VideoFileParserFFmpeg::GetNextFrame(std::vector<uint8_t>& imageData) {
  AVFrame* frame{nullptr};

  // try to read frame from current packet
  bool frameReceived = decoder_->receiveDecodedFrame(&frame);

  // try again with new packet if no frame received
  while (!frameReceived && !decoder_->endOfFileReached()) {
    AVPacket* packet{nullptr};
    if (!demuxer_.readFrame(&packet)) {
      return false;
    }
    decoder_->sendPacketForDecoding(packet);

    // fetch new frame from sent packet
    frameReceived = decoder_->receiveDecodedFrame(&frame);
  }

  // copy received frame (converted to RGB24) to imageData vector
  if (frameReceived) {
    scaler_->scaleFrame(frame, &imageData);
  }

  return !decoder_->endOfFileReached();
}
