#include <s3d/video/capture/file_video_capture_device_3d.h>
#include <s3d/video/capture/file_video_capture_device_ffmpeg.h>

#include <fstream>

class FFmpegClient : public VideoCaptureDevice::Client {
 public:
  explicit FFmpegClient(const std::string& dstFilename)
      : frame_{}, dstFilename_{dstFilename}, dstFile_{dstFilename, std::ios::binary} {}

  VideoCaptureDevice::Client* clone() const override { return new FFmpegClient{dstFilename_}; }

 public:
  void OnIncomingCapturedData(const Images& images,
                              const VideoCaptureFormat& frameFormat,
                              std::chrono::microseconds /*timestamp*/) override {
    std::copy(std::begin(images[0]), std::end(images[0]), std::ostreambuf_iterator<char>{dstFile_});
    std::cout << ++frameCount_ << std::endl;
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
  std::unique_ptr<VideoCaptureDevice::Client> client = std::make_unique<FFmpegClient>(src_filename);

  // create video capture device
  std::unique_ptr<VideoCaptureDevice> captureDevice = std::make_unique<FileVideoCaptureDevice3D>(
      "/home/jon/Videos/bbb_sunflower_1080p_30fps_stereo_left.mp4;"
      "/home/jon/Videos/bbb_sunflower_1080p_30fps_stereo_right.mp4");

  // start "capture"
  VideoCaptureFormat format{{}, -1.0f, VideoPixelFormat::BGR};
  captureDevice->AllocateAndStart(format, client.get());

  //  char c = ' ';
  //  while (c != 'q') {
  //    {
  //      std::unique_lock<std::mutex> g_ioMutex;
  //      std::cin.get(c);
  //    }
  //
  //    if (c == 'p') {
  //      captureDevice->MaybeSuspend();
  //    } else if (c == 'r') {
  //      captureDevice->Resume();
  //    }
  //  }
  captureDevice->StopAndDeAllocate();

  std::cout << "Play the output video file with the command:" << std::endl
            << "ffplay -f rawvideo -pixel_format rgb24 -video_size 1920x1080 -framerate 30 "
            << video_dst_filename << std::endl;

  return 0;
}
