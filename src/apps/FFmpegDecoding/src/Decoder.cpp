#include "Decoder.h"

Decoder::Decoder(AVFormatContext* formatContext) : formatContext_{formatContext} {
  streamIndex_ = openCodexContext(codecContext_, formatContext, AVMEDIA_TYPE_VIDEO);
  frame_ = ffmpeg::UniquePtr<AVFrame>(ffmpeg::avframe::alloc());
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

size_t Decoder::frameBufferSize() {
  return av_image_get_buffer_size(codecContext_->pix_fmt, codecContext_->width,
                                  codecContext_->height, 1);
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
