#include "ffmpeg_utils.h"

namespace ffmpeg {

namespace avformat {
void open_input(AVFormatContext** ps, const char* url, AVInputFormat* fmt, AVDictionary** options) {
  /* open input file, and allocate format context */
  if (avformat_open_input(ps, url, fmt, options) < 0) {
    throw FFmpegException(std::string("Could not open source file ") + std::string(url));
  }
}

void find_stream_info(AVFormatContext* ic, AVDictionary** options) {
  if (avformat_find_stream_info(ic, options) < 0) {
    throw FFmpegException("Could not find stream information");
  }
}

int find_best_stream(AVFormatContext* formatContext,
                     enum AVMediaType type)
{
  return find_best_stream(formatContext, type, -1, -1, nullptr, 0);
}

int find_best_stream(AVFormatContext* formatContext,
                     enum AVMediaType type,
                     int wanted_stream_nb,
                     int related_stream,
                     AVCodec** decoder_ret,
                     int flags) {
  int ret = av_find_best_stream(formatContext, type, wanted_stream_nb, related_stream, decoder_ret, flags);
  if (ret < 0) {
    throw FFmpegException("Could not find " +
        std::string(av_get_media_type_string(type)) +
        std::string(" stream in input file") +
        std::string(av_get_media_type_string(type)));
  }
  return ret;
}

}  // namespace avformat

namespace avcodec {

AVCodec* find_decoder(AVStream *stream) {
  return find_decoder(stream->codecpar->codec_id);
}

AVCodec* find_decoder(enum AVCodecID id) {
  AVCodec* dec = avcodec_find_decoder(id);
  if (!dec) {
    throw FFmpegException("Failed to find codec");
  }
  return dec;
}

AVCodecContext* alloc_context3(const AVCodec* codec) {
  AVCodecContext* context = avcodec_alloc_context3(codec);
  if (context == nullptr) {
    throw FFmpegException("Failed to allocate the codec context");
  }
  return context;
}

void parameters_to_context(AVCodecContext* codecContext, const AVCodecParameters* parameters) {
  if (avcodec_parameters_to_context(codecContext, parameters) < 0) {
    throw FFmpegException("Failed to copy codec parameters to decoder context");
  }
}


void open2(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options)
{
  if (avcodec_open2(avctx, codec, options) < 0) {
    throw FFmpegException("Failed to open codec");
  }
}


}  // namespace avcodec

}  // namespace ffmpeg
