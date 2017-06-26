#include "s3d/video/file_parser/ffmpeg/ffmpeg_utils.h"

namespace ffmpeg {

void avformat::open_input(AVFormatContext** ps,
                          const char* url,
                          AVInputFormat* fmt,
                          AVDictionary** options) {
  /* open input file, and allocate format context */
  if (avformat_open_input(ps, url, fmt, options) < 0) {
    throw FFmpegException(std::string("Could not open source file ") + std::string(url));
  }
}

UniquePtr<AVFormatContext> avformat::open_input(const char* url,
                                                AVInputFormat* fmt,
                                                AVDictionary** options) {
  AVFormatContext* ps{nullptr};
  /* open input file, and allocate format context */
  if (avformat_open_input(&ps, url, fmt, options) < 0) {
    throw FFmpegException(std::string("Could not open source file ") + std::string(url));
  }
  return ffmpeg::UniquePtr<AVFormatContext>(ps);
}

void avformat::find_stream_info(AVFormatContext* ic, AVDictionary** options) {
  if (avformat_find_stream_info(ic, options) < 0) {
    throw FFmpegException("Could not find stream information");
  }
}

int avformat::find_best_stream(AVFormatContext* formatContext, enum AVMediaType type) {
  return find_best_stream(formatContext, type, -1, -1, nullptr, 0);
}

int avformat::find_best_stream(AVFormatContext* formatContext,
                               enum AVMediaType type,
                               int wanted_stream_nb,
                               int related_stream,
                               AVCodec** decoder_ret,
                               int flags) {
  int ret = av_find_best_stream(formatContext, type, wanted_stream_nb, related_stream, decoder_ret,
                                flags);
  if (ret < 0) {
    throw FFmpegException("Could not find " + std::string(av_get_media_type_string(type)) +
                          std::string(" stream in input file") +
                          std::string(av_get_media_type_string(type)));
  }
  return ret;
}

bool avformat::read_frame(AVFormatContext* s, AVPacket* pkt) {
  return av_read_frame(s, pkt) >= 0;
}

AVCodec* avcodec::find_decoder(AVStream* stream) {
  return find_decoder(stream->codecpar->codec_id);
}

AVCodec* avcodec::find_decoder(enum AVCodecID id) {
  AVCodec* dec = avcodec_find_decoder(id);
  if (dec == nullptr) {
    throw FFmpegException("Failed to find codec");
  }
  return dec;
}

AVCodecContext* avcodec::alloc_context3(const AVCodec* codec) {
  AVCodecContext* context = avcodec_alloc_context3(codec);
  if (context == nullptr) {
    throw FFmpegException("Failed to allocate the codec context");
  }
  return context;
}

void avcodec::parameters_to_context(AVCodecContext* codecContext,
                                    const AVCodecParameters* parameters) {
  if (avcodec_parameters_to_context(codecContext, parameters) < 0) {
    throw FFmpegException("Failed to copy codec parameters to decoder context");
  }
}

void avcodec::open2(AVCodecContext* avctx, const AVCodec* codec, AVDictionary** options) {
  if (avcodec_open2(avctx, codec, options) < 0) {
    throw FFmpegException("Failed to open codec");
  }
}

attribute_deprecated int avcodec::decode_video2(AVCodecContext* avctx,
                                                AVFrame* picture,
                                                int* got_picture_ptr,
                                                const AVPacket* avpkt) {
  int ret = avcodec_decode_video2(avctx, picture, got_picture_ptr, avpkt);
  if (ret < 0) {
    char errBuff[AV_ERROR_MAX_STRING_SIZE];
    av_make_error_string(errBuff, AV_ERROR_MAX_STRING_SIZE, ret);
    throw FFmpegException(std::string("Error decoding video frame: ") + errBuff);
  }
  return ret;
}

bool avcodec::send_packet(AVCodecContext* avctx, const AVPacket* avpkt, bool* inputBufferFull) {
  int res = avcodec_send_packet(avctx, avpkt);
  if (res < 0 && res != AVERROR_EOF) {
    char errBuff[AV_ERROR_MAX_STRING_SIZE];
    av_make_error_string(errBuff, AV_ERROR_MAX_STRING_SIZE, res);
    throw FFmpegException("Cannot send packet to decoder: " + std::string(errBuff));
  }
  *inputBufferFull = (res == AVERROR(EAGAIN));
  // true if can continue, false if end of file
  return res != AVERROR_EOF;
}

bool avcodec::receive_frame(AVCodecContext* avctx, AVFrame* frame, bool* receiveQueueEmpty) {
  int res = avcodec_receive_frame(avctx, frame);
  if (res < 0 && res != AVERROR_EOF && res != AVERROR(EAGAIN)) {
    char errBuff[AV_ERROR_MAX_STRING_SIZE];
    av_make_error_string(errBuff, AV_ERROR_MAX_STRING_SIZE, res);
    throw FFmpegException("Cannot receive packet from decoder: " + std::string(errBuff));
  }
  *receiveQueueEmpty = (res == AVERROR(EAGAIN));
  // true if can continue, false if end of file
  return res != AVERROR_EOF;
}

int imgutils::image_alloc(uint8_t** pointers,
                          int linesizes[4],
                          int w,
                          int h,
                          enum AVPixelFormat pix_fmt,
                          int align) {
  int buffer_size = av_image_alloc(pointers, linesizes, w, h, pix_fmt, align);
  if (buffer_size < 0) {
    throw FFmpegException("Could not allocate raw video buffer");
  }
  return buffer_size;
}

AVFrame* avframe::alloc() {
  auto* frame = av_frame_alloc();
  if (frame == nullptr) {
    throw FFmpegException("Could not allocate frame");
  }
  return frame;
}

void avpacket::init(AVPacket* pkt) {
  av_init_packet(pkt);
  pkt->data = nullptr;
  pkt->size = 0;
}

}  // namespace ffmpeg
