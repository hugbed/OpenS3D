/*
 * Copyright (c) 2012 Stefano Sabatini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file
 * Demuxing and decoding example.
 *
 * Show how to use the libavformat and libavcodec API to demux and
 * decode video data.
 * @example demuxing_decoding.c
 */

#include <stdexcept>

#include <stdexcept>

class BadNumberOfInputArgs : public std::runtime_error {
 public:
  explicit BadNumberOfInputArgs(const std::string& programName)
      : std::runtime_error(
            std::string("usage: ") + programName + std::string(" input_file video_output_file\n") +
            std::string(
                "       API example program to show how to read frames from an input file.\n"
                "       This program reads frames from a file, decodes them, and writes decoded\n"
                "       video frames to a rawvideo file named video_output_file, and decoded\n")) {}
};

#include <iostream>

#include "ffmpeg_utils.h"

struct Decoder {
  AVFormatContext* formatContext_{nullptr};
  ffmpeg::AVCodecContextPtr codecContext_{nullptr};
  int width, height;
  enum AVPixelFormat pix_fmt;
  AVStream* video_stream{nullptr};
  FILE* video_dst_file{nullptr};

  uint8_t* video_dst_data[4]{nullptr};
  int video_dst_linesize[4];
  int video_dst_bufsize;

  int video_stream_idx{-1};
  AVFrame* frame{nullptr};
  AVPacket pkt;
  int video_frame_count{0};

  int decode_packet(int* got_frame, int cached) {
    int ret = 0;

    *got_frame = 0;

    if (pkt.stream_index == video_stream_idx) {
      /* decode video frame */
      ret = avcodec_decode_video2(codecContext_.get(), frame, got_frame, &pkt);
      if (ret < 0) {
        fprintf(stderr, "Error decoding video frame (%s)\n", av_err2str(ret));
        return ret;
      }

      if (*got_frame) {
        if (frame->width != width || frame->height != height || frame->format != pix_fmt) {
          /* To handle this change, one could call av_image_alloc again and
           * decode the following frames into another rawvideo file. */
          fprintf(stderr,
                  "Error: Width, height and pixel format have to be "
                  "constant in a rawvideo file, but the width, height or "
                  "pixel format of the input video changed:\n"
                  "old: width = %d, height = %d, format = %s\n"
                  "new: width = %d, height = %d, format = %s\n",
                  width, height, av_get_pix_fmt_name(pix_fmt), frame->width, frame->height,
                  av_get_pix_fmt_name(static_cast<AVPixelFormat>(frame->format)));
          return -1;
        }

        printf("video_frame%s n:%d coded_n:%d\n", cached ? "(cached)" : "", video_frame_count++,
               frame->coded_picture_number);

        /* copy decoded frame to destination buffer:
         * this is required since rawvideo expects non aligned data */
        av_image_copy(video_dst_data, video_dst_linesize, (const uint8_t**)(frame->data),
                      frame->linesize, pix_fmt, width, height);

        /* write to rawvideo file */
        fwrite(video_dst_data[0], 1, video_dst_bufsize, video_dst_file);
      }
    }

    return ret;
  }

  static int open_codec_context(ffmpeg::AVCodecContextPtr& codecContext,
                                AVFormatContext* formatContext,
                                enum AVMediaType type) {

    using namespace ffmpeg::avformat;
    using namespace ffmpeg::avcodec;

    // find stream
    int stream_index = find_best_stream(formatContext, type);
    AVStream* stream = formatContext->streams[stream_index];

    // create codec context
    AVCodec* codec = find_decoder(stream);
    codecContext = ffmpeg::AVCodecContextPtr(alloc_context3(codec));
    parameters_to_context(codecContext.get(), stream->codecpar);
    open2(codecContext.get(), codec, nullptr);

    return stream_index;
  }

  int init(const char* src_filename, const char* video_dst_filename) {
    int ret = 0, got_frame;

    av_register_all();

    ffmpeg::avformat::open_input(&formatContext_, src_filename, nullptr, nullptr);
    ffmpeg::avformat::find_stream_info(formatContext_, nullptr);

    video_stream_idx = open_codec_context(codecContext_, formatContext_, AVMEDIA_TYPE_VIDEO);
    video_stream = formatContext_->streams[video_stream_idx];

    video_dst_file = fopen(video_dst_filename, "wb");
    if (video_dst_file == nullptr) {
      fprintf(stderr, "Could not open destination file %s\n", video_dst_filename);
      cleanup();
      return -1;
    }

    /* allocate image where the decoded image will be put */
    width = codecContext_->width;
    height = codecContext_->height;
    pix_fmt = codecContext_->pix_fmt;
    ret = av_image_alloc(video_dst_data, video_dst_linesize, width, height, pix_fmt, 1);
    if (ret < 0) {
      fprintf(stderr, "Could not allocate raw video buffer\n");
      cleanup();
      return -1;
    }
    video_dst_bufsize = ret;

    /* dump input information to stderr */
    av_dump_format(formatContext_, 0, src_filename, 0);

    if (!video_stream) {
      fprintf(stderr, "Could not find video stream in the input, aborting\n");
      cleanup();
      return -1;
    }

    frame = av_frame_alloc();
    if (!frame) {
      fprintf(stderr, "Could not allocate frame\n");
      ret = AVERROR(ENOMEM);
      cleanup();
      return -1;
    }

    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;

    if (video_stream)
      printf("Demuxing video from file '%s' into '%s'\n", src_filename, video_dst_filename);

    /* read frames from the file */
    while (av_read_frame(formatContext_, &pkt) >= 0) {
      AVPacket orig_pkt = pkt;
      do {
        ret = decode_packet(&got_frame, 0);
        if (ret < 0)
          break;
        pkt.data += ret;
        pkt.size -= ret;
      } while (pkt.size > 0);
      av_packet_unref(&orig_pkt);
    }

    /* flush cached frames */
    pkt.data = nullptr;
    pkt.size = 0;
    do {
      decode_packet(&got_frame, 1);
    } while (got_frame);

    printf("Demuxing succeeded.\n");

    if (video_stream) {
      printf(
          "Play the output video file with the command:\n"
          "ffplay -f rawvideo -pix_fmt %s -video_size %dx%d %s\n",
          av_get_pix_fmt_name(pix_fmt), width, height, video_dst_filename);
    }

    return ret;
  }

  void cleanup() {
    codecContext_.reset();
    avformat_close_input(&formatContext_);
    if (video_dst_file)
      fclose(video_dst_file);
    av_frame_free(&frame);
    av_free(video_dst_data[0]);
  }

  ~Decoder() { cleanup(); }
};

void checkArguments(int argc, char** argv) {
  if (argc != 3) {
    throw BadNumberOfInputArgs(std::string(argv[0]));
  }
}

int main(int argc, char** argv) {
  checkArguments(argc, argv);
  const char* src_filename = argv[1];
  const char* video_dst_filename = argv[2];

  Decoder decoder;
  return decoder.init(argv[1], argv[2]) < 0;
}

// openFile(filename)
// retrieveStreamInfo()
// openDecoder()
