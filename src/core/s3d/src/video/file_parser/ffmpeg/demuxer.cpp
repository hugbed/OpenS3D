#include "s3d/video/file_parser/ffmpeg/demuxer.h"

#include "s3d/video/file_parser/ffmpeg/decoder.h"

Demuxer::Demuxer(const std::string& inputFilename) {
  using namespace ffmpeg;
  av_register_all();

  formatContext_ = avformat::open_input(inputFilename.c_str());
  avformat::find_stream_info(formatContext_.get());
  avpacket::init(&packet_);
  av_dump_format(formatContext_.get(), 0, inputFilename.c_str(), 0);
}

bool Demuxer::readFrame(AVPacket** packet) {
  *packet = &packet_;
  return ffmpeg::avformat::read_frame(formatContext_.get(), &packet_);
}

std::unique_ptr<Decoder> Demuxer::createDecoder() {
  return std::make_unique<Decoder>(formatContext_.get());
}
