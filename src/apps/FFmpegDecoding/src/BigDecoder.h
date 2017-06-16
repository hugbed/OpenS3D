//
// Created by jon on 15/06/17.
//

#ifndef PROJECT_BIGDECODER_H
#define PROJECT_BIGDECODER_H

#include "ffmpeg_utils.h"

class BigDecoder {
 public:
  BigDecoder(const std::string& videoSrcFilename, const std::string& videoDestinationFilename);
  ~BigDecoder();

  bool getNextFrame();

  void displaySuccessMessage(const std::string& video_dst_filename);

 private:
  bool init(const std::string& src_filename, const std::string& video_dst_filename);
  void initRGB();

  int decode_packet(int* got_frame, bool cached);
  static int open_codec_context(ffmpeg::UniquePtr<AVCodecContext>& codecContext,
                                AVFormatContext* formatContext,
                                enum AVMediaType type);
  bool frameHasSameProperties(AVFrame* otherFrame);
  static bool isFromVideoStream(AVPacket pkt, int videoStreamIdx);
  void displayCurrentFrameDecoded(bool cached);
  void flushCachedFrames(AVPacket* pkt);
  void allocateDecodedImage();

  // lots of variables
  ffmpeg::UniquePtr<AVFormatContext> formatContext_{nullptr};
  ffmpeg::UniquePtr<AVCodecContext> codecContext_{nullptr};
  ffmpeg::UniquePtr<FILE> video_dst_file{nullptr};
  AVStream* video_stream{nullptr};

  // AVFrame
  uint8_t* video_dst_data[4]{nullptr};
  int width{0}, height{0};
  enum AVPixelFormat pix_fmt { AV_PIX_FMT_NONE };
  int video_dst_linesize[4]{};
  int video_dst_bufsize{0};

  int video_stream_idx{-1};
  ffmpeg::UniquePtr<AVFrame> frame{nullptr};

  AVPacket pkt{};
  int video_frame_count{0};
};

#endif  // PROJECT_BIGDECODER_H
