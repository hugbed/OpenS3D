#ifndef S3D_VIDEO_FILE_PARSER_FFMPEG_FFMPEG_UTILS_H
#define S3D_VIDEO_FILE_PARSER_FFMPEG_FFMPEG_UTILS_H

extern "C" {
#include <cstdint>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include "s3d/video/video_types.h"

#include <stdexcept>
#include <memory>

namespace s3d {

namespace ffmpeg {

AVPixelFormat pixelFormatToAV(VideoPixelFormat pixelFormat);

// todo, have derived (more specific) exceptions
struct FFmpegException : std::runtime_error {
  explicit FFmpegException(const std::string& what) : std::runtime_error(what) {}
};

template <class T>
struct AVDeleter {
  void operator()(T* p) { av_free(p); }
};

template <>
struct AVDeleter<AVCodecContext> {
  void operator()(AVCodecContext* p) { avcodec_free_context(&p); }
};

template <>
struct AVDeleter<AVFrame> {
  void operator()(AVFrame* p) { av_frame_free(&p); }
};

template <>
struct AVDeleter<AVFormatContext> {
  void operator()(AVFormatContext* p) { avformat_close_input(&p); }
};

template <>
struct AVDeleter<FILE> {
  void operator()(FILE* p) { fclose(p); }
};

template <>
struct AVDeleter<uint8_t> {
  void operator()(uint8_t* p) { av_freep(&p); }
};

template <>
struct AVDeleter<SwsContext> {
  void operator()(SwsContext* p) { sws_freeContext(p); }
};

template <class T>
using UniquePtr = std::unique_ptr<T, AVDeleter<T>>;

namespace avformat {

void open_input(AVFormatContext** ps,
                const char* url,
                AVInputFormat* fmt = nullptr,
                AVDictionary** options = nullptr);

UniquePtr<AVFormatContext> open_input(const char* url,
                                      AVInputFormat* fmt = nullptr,
                                      AVDictionary** options = nullptr);

void find_stream_info(AVFormatContext* formatContext, AVDictionary** options = nullptr);

int find_best_stream(AVFormatContext* formatContext, enum AVMediaType type);

int find_best_stream(AVFormatContext* formatContext,
                     enum AVMediaType type,
                     int wanted_stream_nb,
                     int related_stream,
                     AVCodec** decoder_ret,
                     int flags);

bool read_frame(AVFormatContext* s, AVPacket* pkt);

bool seek_frame(AVFormatContext* s, int stream_index, int64_t timestamp, int flags);

}  // namespace avformat

namespace avcodec {

AVCodec* find_decoder(AVStream* stream);

AVCodec* find_decoder(enum AVCodecID id);

AVCodecContext* alloc_context3(const AVCodec* codec);

void parameters_to_context(AVCodecContext* codec, const AVCodecParameters* par);

void open2(AVCodecContext* avctx, const AVCodec* codec, AVDictionary** options);

int decode_video2(AVCodecContext* avctx,
                  AVFrame* picture,
                  int* got_picture_ptr,
                  const AVPacket* avpkt);

bool send_packet(AVCodecContext* avctx, const AVPacket* avpkt, bool* shouldConsumePackets);

bool receive_frame(AVCodecContext* avctx, AVFrame* frame, bool* shouldSendNextPacket);

}  // namespace avcodec

namespace imgutils {

int image_alloc(uint8_t* pointers[4],
                int linesizes[4],
                int w,
                int h,
                enum AVPixelFormat pix_fmt,
                int align);

}  // namespace imgutils

namespace avframe {
AVFrame* alloc();
}  // namespace avframe

namespace avpacket {
void init(AVPacket* pkt);
}  // namespace avpacket

}  // namespace ffmpeg

}  // namespace s3d

#endif  // S3D_VIDEO_FILE_PARSER_FFMPEG_FFMPEG_UTILS_H
