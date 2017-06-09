#ifndef PROJECT_FFMPEG_UTILS_H
#define PROJECT_FFMPEG_UTILS_H


extern "C" {
#include <cstdint>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
}

#include <stdexcept>
#include <memory>

namespace ffmpeg {

// todo, have derived (more specific) exceptions
struct FFmpegException : std::runtime_error {
  explicit FFmpegException(const std::string& what) : std::runtime_error(what) {}
};

struct AVCodecContextDeleter {
  void operator()(AVCodecContext* context) {
    avcodec_free_context(&context);
  }
};

using AVCodecContextPtr = std::unique_ptr<AVCodecContext, AVCodecContextDeleter>;

namespace avformat {

void open_input(AVFormatContext** ps, const char* url, AVInputFormat* fmt, AVDictionary** options);

void find_stream_info(AVFormatContext* ic, AVDictionary** options);

int find_best_stream(AVFormatContext* ic,
                     enum AVMediaType type);

int find_best_stream(AVFormatContext* ic,
                     enum AVMediaType type,
                     int wanted_stream_nb,
                     int related_stream,
                     AVCodec** decoder_ret,
                     int flags);

}  // namespace avformat

namespace avcodec {

AVCodec* find_decoder(AVStream* stream);

AVCodec* find_decoder(enum AVCodecID id);

AVCodecContext* alloc_context3(const AVCodec* codec);

void parameters_to_context(AVCodecContext* codec, const AVCodecParameters* par);

void open2(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);

}  // namespace avcodec

}  // namespace ffmpeg

#endif  // PROJECT_FFMPEG_UTILS_H
