#include "gtest/gtest.h"

#include "s3d/video/file_parser/raw_uyvy_file_parser.h"

#include "s3d/video/capture/video_capture_types.h"

#include <istream>

class FakeRawUYVYFileParser : public RawUYVYFileParser {
 public:
  explicit FakeRawUYVYFileParser(std::string filePath) : RawUYVYFileParser(std::move(filePath)) {}

  bool createStream(std::unique_ptr<std::istream>& stream) override {
    std::unique_ptr<std::istream> s = std::make_unique<std::istringstream>("hello");
    stream = std::move(s);
    return true;
  }
};

TEST(raw_uyvy_file_parser, default_timestamp_is_default) {
  FakeRawUYVYFileParser parser("");
  EXPECT_EQ(parser.CurrentFrameTimestamp().count(), std::chrono::microseconds{}.count());
}

TEST(raw_uyvy_file_parser, default_video_duration_is_zero) {
  FakeRawUYVYFileParser parser("");
  EXPECT_EQ(parser.VideoDuration().count(), std::chrono::microseconds(0).count());
}

TEST(raw_uyvy_file_parser, file_path_correctly_set) {
  std::string path{"somepath.txt"};
  RawUYVYFileParser parser(path);
  EXPECT_STREQ(path.c_str(), parser.getFilePath().c_str());
}

TEST(raw_uyvy_file_parser, initializes_correctly) {
  VideoCaptureFormat format;
  FakeRawUYVYFileParser parser("somepath.txt");
  bool res = parser.Initialize(&format);
  EXPECT_TRUE(res);
}

TEST(raw_uyvy_file_parser, get_next_frame_hardcoded_size_and_format) {
  VideoCaptureFormat format;
  FakeRawUYVYFileParser parser("somepath.txt");
  parser.Initialize(&format);

  std::vector<uint8_t> frame;
  parser.GetNextFrame(&frame);
  EXPECT_EQ(frame.size(), 1920 * 1080 * 3);
}

TEST(raw_uyvy_file_parser, create_stream_file_not_found) {
  VideoCaptureFormat format;
  RawUYVYFileParser parser("file_not_found.txt");
  EXPECT_FALSE(parser.Initialize(&format));
}

TEST(video_file_parser, seek_to_frame_does_nothing) {
  VideoCaptureFormat format;
  RawUYVYFileParser parser("file_not_found.txt");
  parser.SeekToFrame({});
}

TEST(raw_uyvy_file_parser, get_next_frame_nullptr_returns_false) {
  VideoCaptureFormat format;
  FakeRawUYVYFileParser parser("somepath.txt");
  EXPECT_FALSE(parser.GetNextFrame(nullptr));
}
