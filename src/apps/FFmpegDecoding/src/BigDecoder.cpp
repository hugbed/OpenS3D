#include "BigDecoder.h"

#include <iostream>

BigDecoder::BigDecoder(const std::string& videoSrcFilename,
                       const std::string& videoDestinationFilename) {
  init(videoSrcFilename, videoDestinationFilename);
}

BigDecoder::~BigDecoder() {
  if (video_dst_data[0] != nullptr) {
    av_freep(&video_dst_data[0]);
  }
}

bool BigDecoder::getNextFrame() {
  int res = av_read_frame(formatContext_.get(), &pkt);

  if (res >= 0) {
    AVPacket orig_pkt = pkt;
    do {
      int got_frame{}, buf_size{};
      buf_size = decode_packet(&got_frame, false);
      pkt.data += buf_size;
      pkt.size -= buf_size;
    } while (pkt.size > 0);
    av_packet_unref(&orig_pkt);

    return true;
  }  // else error or end of file

  flushCachedFrames(&pkt);

  return false;
}

void BigDecoder::displaySuccessMessage(const std::string& video_dst_filename) {
  printf("Demuxing succeeded.\n");

  if (video_stream) {
    printf(
        "Play the output video file with the command:\n"
        "ffplay -f rawvideo -pix_fmt %s -video_size %dx%d %s\n",
        av_get_pix_fmt_name(pix_fmt), width, height, video_dst_filename.c_str());
  }
}

bool BigDecoder::init(const std::string& src_filename, const std::string& video_dst_filename) {
  av_register_all();

  // open file and find streams
  AVFormatContext* rawFormatContext{nullptr};
  ffmpeg::avformat::open_input(&rawFormatContext, src_filename.c_str(), nullptr, nullptr);
  formatContext_ = ffmpeg::UniquePtr<AVFormatContext>(rawFormatContext);
  ffmpeg::avformat::find_stream_info(formatContext_.get(), nullptr);

  video_stream_idx = open_codec_context(codecContext_, formatContext_.get(), AVMEDIA_TYPE_VIDEO);
  video_stream = formatContext_->streams[video_stream_idx];

  av_dump_format(formatContext_.get(), 0, src_filename.c_str(), 0);

  if (!video_stream) {
    std::cerr << "Could not find video stream in the input, aborting" << std::endl;
    return false;
  }

  video_dst_file = ffmpeg::UniquePtr<FILE>(fopen(video_dst_filename.c_str(), "wb"));

  if (video_dst_file == nullptr) {
    std::cerr << "Could not open destination file " << video_dst_filename << std::endl;
    return false;
  }

  // allocations, initializations
  allocateDecodedImage();
  frame = ffmpeg::UniquePtr<AVFrame>(ffmpeg::avframe::alloc());
  ffmpeg::avpacket::init(&pkt);  // initialize packet, set data to NULL, let the demuxer fill it

  return true;
}

int BigDecoder::decode_packet(int* got_frame, bool cached) {
  int frame_size = 0;

  if (isFromVideoStream(pkt, video_stream_idx)) {
    /* decode video frame */
    *got_frame = 0;
    frame_size = ffmpeg::avcodec::decode_video2(codecContext_.get(), frame.get(), got_frame, &pkt);

    if (*got_frame != 0) {
      if (!frameHasSameProperties(frame.get()))
        return -1;
      displayCurrentFrameDecoded(cached);

      /* copy decoded frame to destination buffer:
       * this is required since rawvideo expects non aligned data */
      av_image_copy(video_dst_data, video_dst_linesize, const_cast<const uint8_t**>(frame->data),
                    frame->linesize, pix_fmt, width, height);

      /* write to rawvideo file */
      fwrite(video_dst_data[0], 1, video_dst_bufsize, video_dst_file.get());
    }
  }

  return frame_size;
}

// static
int BigDecoder::open_codec_context(ffmpeg::UniquePtr<AVCodecContext>& codecContext,
                                   AVFormatContext* formatContext,
                                   enum AVMediaType type) {
  using namespace ffmpeg::avformat;
  using namespace ffmpeg::avcodec;

  // find stream
  int stream_index = find_best_stream(formatContext, type);
  AVStream* stream = formatContext->streams[stream_index];

  // create codec context
  AVCodec* codec = find_decoder(stream);
  codecContext = ffmpeg::UniquePtr<AVCodecContext>(alloc_context3(codec));
  parameters_to_context(codecContext.get(), stream->codecpar);
  open2(codecContext.get(), codec, nullptr);

  return stream_index;
}

bool BigDecoder::frameHasSameProperties(AVFrame* otherFrame) {
  bool ret = !(otherFrame->width != width || otherFrame->height != height ||
               otherFrame->format != pix_fmt);
  if (!ret) {
    /* To handle this change, one could call av_image_alloc again and
     * decode the following frames into another rawvideo file. */
    fprintf(stderr,
            "Error: Width, height and pixel format have to be "
            "constant in a rawvideo file, but the width, height or "
            "pixel format of the input video changed:\n"
            "old: width = %d, height = %d, format = %s\n"
            "new: width = %d, height = %d, format = %s\n",
            width, height, av_get_pix_fmt_name(pix_fmt), otherFrame->width, otherFrame->height,
            av_get_pix_fmt_name(static_cast<AVPixelFormat>(otherFrame->format)));
  }
  return ret;
}

// static
bool BigDecoder::isFromVideoStream(AVPacket pkt, int videoStreamIdx) {
  return pkt.stream_index == videoStreamIdx;
}

void BigDecoder::displayCurrentFrameDecoded(bool cached) {
  printf("video_frame%s n:%d coded_n:%d\n", cached ? "(cached)" : "", video_frame_count++,
         frame->coded_picture_number);
}

void BigDecoder::flushCachedFrames(AVPacket* pkt) {
  int got_frame;
  pkt->data = nullptr;
  pkt->size = 0;
  do {
    decode_packet(&got_frame, true);
  } while (got_frame != 0);
}

void BigDecoder::allocateDecodedImage() {
  width = codecContext_->width;
  height = codecContext_->height;
  pix_fmt = codecContext_->pix_fmt;
  video_dst_bufsize =
      ffmpeg::imgutils::image_alloc(video_dst_data, video_dst_linesize, width, height, pix_fmt, 1);
}