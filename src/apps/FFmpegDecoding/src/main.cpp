/**
 * @file
 * Demuxing and decoding example.
 *
 * Show how to use the libavformat and libavcodec API to demux and
 * decode video data.
 * @example demuxing_decoding.c
 */

#include "VideoFileParserFFmpeg.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <thread>

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
  std::vector<uint8_t> frame;
  std::ofstream dstFile{video_dst_filename, std::ios::binary};
  while (parser.GetNextFrame(frame)) {
    std::copy(std::begin(frame), std::end(frame), std::ostreambuf_iterator<char>{dstFile});
    std::cout << "decoded frame " << frameCount++ << std::endl;
  }

  // todo: get format from VideofileParserFFmpeg
  std::cout << "Play the output video file with the command:" << std::endl
            << "ffplay -f rawvideo -pixel_format yuv422p -video_size 1920x1080 -framerate 60 "
            << video_dst_filename << std::endl;

  return 0;
}
