/**
 * @file
 * Demuxing and decoding example.
 *
 * Show how to use the libavformat and libavcodec API to demux and
 * decode video data.
 * @example demuxing_decoding.c
 */

#include "VideoFileParserFFmpeg.h"

#include <s3d/video/capture/file_video_capture_device.h>

#include <memory>
#include <fstream>
#include <thread>

class FileVideoCaptureDeviceFFmpeg : public FileVideoCaptureDevice {
 public:
  FileVideoCaptureDeviceFFmpeg(const std::string& filename)
      : filename_(filename), FileVideoCaptureDevice(filename) {}

  gsl::owner<FileVideoCaptureDeviceFFmpeg*> clone() const override {
    return new FileVideoCaptureDeviceFFmpeg(filename_);
  }

  std::unique_ptr<VideoFileParser> GetVideoFileParser(const std::string& filePath,
                                                      VideoCaptureFormat* format) override {
    auto fileParser =
        std::unique_ptr<VideoFileParser>(std::make_unique<VideoFileParserFFmpeg>(filePath));

    if (!fileParser->Initialize(format)) {
      fileParser.reset();
    }

    return fileParser;
  }

 private:
  std::string filename_;
};

class FFmpegClient : public VideoCaptureDevice::Client {
 public:
  FFmpegClient(const std::string& dstFilename)
    : frame_{}
    , dstFilename_{dstFilename}
    , dstFile_{dstFilename, std::ios::binary} {}

  VideoCaptureDevice::Client* clone() const override { return new FFmpegClient{dstFilename_}; }

 public:
  void OnIncomingCapturedData(const Images& images, const VideoCaptureFormat& frameFormat) override {
    std::copy(std::begin(images[0]), std::end(images[0]), std::ostreambuf_iterator<char>{dstFile_});
    std::cout << "decoded frame " << ++frameCount_ << std::endl;
  }
private:
  int frameCount_{0};
  std::string dstFilename_;
  std::vector<uint8_t> frame_;
  std::ofstream dstFile_;
};

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

  // create client
  std::unique_ptr<VideoCaptureDevice::Client> client =
    std::make_unique<FFmpegClient>(video_dst_filename);

  // create video capture device
  std::unique_ptr<VideoCaptureDevice> captureDevice =
    std::make_unique<FileVideoCaptureDeviceFFmpeg>(src_filename);

  // start "capture"
  captureDevice->AllocateAndStart({}, std::move(client));

  std::cout << "Play the output video file with the command:" << std::endl
            << "ffplay -f rawvideo -pixel_format rgb24 -video_size 1920x1080 -framerate 30 "
            << video_dst_filename << std::endl;

  return 0;
}
