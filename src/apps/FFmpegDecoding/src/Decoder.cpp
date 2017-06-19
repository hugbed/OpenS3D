#include "Decoder.h"

#include "Scaler.h"

#include <cassert>

Decoder::Decoder(AVFormatContext* formatContext) : formatContext_{formatContext} {
  streamIndex_ = openCodexContext(codecContext_, formatContext, AVMEDIA_TYPE_VIDEO);
  frame_ = ffmpeg::UniquePtr<AVFrame>(ffmpeg::avframe::alloc());

  // allocate output buffer
  outputBufferSize_ =
      ffmpeg::imgutils::image_alloc(outputBuffer_, outputBufferLineSize_, codecContext_->width,
                                    codecContext_->height, codecContext_->pix_fmt, 1);

  // transfer ownership, will call av_freep(&outputBuffer_[0])
  outputBufferPtr_ = ffmpeg::UniquePtr<uint8_t>(outputBuffer_[0]);
}

bool Decoder::sendPacketForDecoding(AVPacket* packet) {
  if (packet->stream_index != streamIndex_) {
    return false;
  }
  bool sendQueueWasFull;
  endOfFileReached_ = !ffmpeg::avcodec::send_packet(codecContext_.get(), packet, &sendQueueWasFull);
  return !sendQueueWasFull;
}

bool Decoder::receiveDecodedFrame(AVFrame** frame) {
  *frame = frame_.get();
  bool receiveQueueWasEmpty;
  endOfFileReached_ =
      !ffmpeg::avcodec::receive_frame(codecContext_.get(), frame_.get(), &receiveQueueWasEmpty);
  return !receiveQueueWasEmpty;
}

void Decoder::copyFrameData(AVFrame* frame, std::vector<uint8_t>* out) {
  // need to reallocate a frame for different frame properties
  assert(frame->height == codecContext_->height && frame->width == codecContext_->width &&
         frame->format == codecContext_->pix_fmt);

  // necessary, to remove alignment from raw frame
  av_image_copy(&outputBuffer_[0], outputBufferLineSize_, const_cast<const uint8_t**>(frame->data),
                frame->linesize, codecContext_->pix_fmt, frame->width, frame->height);
  out->resize(outputBufferSize_);
  std::copy(outputBufferPtr_.get(), outputBufferPtr_.get() + outputBufferSize_, std::begin(*out));
}

bool Decoder::endOfFileReached() {
  return endOfFileReached_;
}

// static
int Decoder::openCodexContext(ffmpeg::UniquePtr<AVCodecContext>& codecContext,
                              AVFormatContext* formatContext,
                              enum AVMediaType type) {
  using namespace ffmpeg;

  // find stream
  int stream_index = avformat::find_best_stream(formatContext, type);
  AVStream* stream = formatContext->streams[stream_index];

  // create codec context
  AVCodec* codec = avcodec::find_decoder(stream);
  codecContext = ffmpeg::UniquePtr<AVCodecContext>(avcodec::alloc_context3(codec));
  avcodec::parameters_to_context(codecContext.get(), stream->codecpar);
  avcodec::open2(codecContext.get(), codec, nullptr);

  return stream_index;
}

std::unique_ptr<Scaler> Decoder::createScaler(enum AVPixelFormat dstFormat) {
  return std::make_unique<Scaler>(codecContext_.get(), dstFormat);
}
Size Decoder::getImageSize() {
  return Size(frame_->width, frame_->height);
}
