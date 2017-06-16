/**
 * @file
 * Demuxing and decoding example.
 *
 * Show how to use the libavformat and libavcodec API to demux and
 * decode video data.
 * @example demuxing_decoding.c
 */

#include "BigDecoder.h"
#include "Demuxer.h"
#include "Decoder.h"

#include <iostream>
#include <vector>
#include <fstream>

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

class VideoFileParserFFmpeg {
 public:
  VideoFileParserFFmpeg(const std::string& inputFilename) : demuxer_(inputFilename) {
    decoder_ = demuxer_.createDecoder();
  }

  bool GetNextFrame(std::vector<uint8_t>& imageData) {
    AVFrame *frame{nullptr};

    // try to read frame from current packet
    bool frameReceived = decoder_->receiveDecodedFrame(&frame);

    // try again with new packet if no frame received
    while (!frameReceived && !decoder_->endOfFileReached()) {
      AVPacket *packet{nullptr};
      if (!demuxer_.readFrame(&packet)) {
        return false;
      }
      decoder_->sendPacketForDecoding(packet);

      // fetch new frame from sent packet
      frameReceived = decoder_->receiveDecodedFrame(&frame);
    }

    // handle received frame
    if (frameReceived) {
      imageData.resize(2);
      std::cout << "Got a frame, size = " << frame->width << ", " << frame->height << std::endl;
    }

    return !decoder_->endOfFileReached();
  }

 private:
  Demuxer demuxer_;
  std::unique_ptr<Decoder> decoder_;
  bool lastFrameIsValid_{false};
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

  VideoFileParserFFmpeg parser(src_filename);

  int frameCount{1};

  std::ofstream dstFile{"/home/jon/Videos/hola.yuv", std::ios::binary};

  std::vector<uint8_t> frame;
  while (parser.GetNextFrame(frame)) {
      std::copy(std::begin(frame), std::end(frame), std::ostreambuf_iterator<char>{dstFile});
      std::cout << "decoded frame " << frameCount++ << std::endl;
  }

  //    BigDecoder decoder(src_filename, video_dst_filename);
  //
  //    std::cout << "Initialized video demuxing from file "  //
  //              << src_filename << " into "                 //
  //              << video_dst_filename << std::endl;
  //
  //    while (decoder.getNextFrame()) {
  //    }
  //
  //    decoder.displaySuccessMessage(video_dst_filename);

  return 0;
}
