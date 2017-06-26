#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "s3d/video/capture/file_video_capture_device.h"
#include "s3d/video/file_parser/video_file_parser.h"

class FakeTimedLoop : public TimedLoop {
 public:
  FakeTimedLoop() = default;
  gsl::owner<TimedLoop*> clone() const override { return new FakeTimedLoop; }

  void start(Client* client, std::chrono::microseconds loopDuration) override {
    lastLoopDuration_ = loopDuration;
    client->callback();
  }
  void stop() override {}

  std::chrono::microseconds lastLoopDuration_;
};

class MockTimedLoop : public TimedLoop {
 public:
  MockTimedLoop() = default;
  gsl::owner<TimedLoop*> clone() const override { return new MockTimedLoop; }
  MOCK_METHOD2(start, void(Client* client, std::chrono::microseconds loopDuration));
  MOCK_METHOD0(stop, void());
};

class FakeVideoFileParser : public VideoFileParser {
 public:
  FakeVideoFileParser(const VideoCaptureFormat& fakeFileFormat)
      : VideoFileParser(), fakeFileFormat_{fakeFileFormat} {}
  gsl::owner<VideoFileParser*> clone() const override {
    return new FakeVideoFileParser(fakeFileFormat_);
  }
  bool Initialize(VideoCaptureFormat* format) override {
    *format = fakeFileFormat_;
    return true;
  }
  bool GetNextFrame(std::vector<uint8_t>& frame) override { return true; }

  VideoCaptureFormat fakeFileFormat_;
};

class MockVideoFileParser : public VideoFileParser {
 public:
  MockVideoFileParser() : VideoFileParser() {}
  gsl::owner<VideoFileParser*> clone() const override { return new MockVideoFileParser; }
  MOCK_METHOD1(Initialize, bool(VideoCaptureFormat* format));
  MOCK_METHOD1(GetNextFrame, bool(std::vector<uint8_t>& frame));
};

class FakeFileVideoCaptureDevice : public FileVideoCaptureDevice {
 public:
  FakeFileVideoCaptureDevice() : FileVideoCaptureDevice("") {}

  // gtest callback mock is not thread friendly
  void StartCaptureThread() override { FileVideoCaptureDevice::OnAllocateAndStart(); }
};

class FakeVideoCaptureDeviceClient : public VideoCaptureDevice::Client {
 public:
  FakeVideoCaptureDeviceClient() = default;
  gsl::owner<VideoCaptureDevice::Client*> clone() const override {
    return new FakeVideoCaptureDeviceClient;
  }

  void OnIncomingCapturedData(const Images& images,
                              const VideoCaptureFormat& frameFormat) override {
    lastFormat_ = frameFormat;
  }

  VideoCaptureFormat lastFormat_;
};

class MockCaptureLoopClient : public FileVideoCaptureDevice::CaptureLoopClient {
 public:
  MockCaptureLoopClient(FileVideoCaptureDevice* device) : CaptureLoopClient(device) {}
  MOCK_METHOD0(callback, void());
};

TEST(file_video_capture_device, start_starts_loop_and_stop_stops_loop) {
  FakeFileVideoCaptureDevice device;
  auto timedLoopPtr = new MockTimedLoop;
  auto fakeVideoCaptureDeviceClient = std::make_unique<FakeVideoCaptureDeviceClient>();

  using ::testing::_;
  EXPECT_CALL(*timedLoopPtr, start(_, _));
  device.Start(VideoCaptureFormat{}, fakeVideoCaptureDeviceClient.get(),
               std::make_unique<FileVideoCaptureDevice::CaptureLoopClient>(&device),
               std::make_unique<FakeVideoFileParser>(VideoCaptureFormat{}),
               std::unique_ptr<MockTimedLoop>(timedLoopPtr));
  EXPECT_CALL(*timedLoopPtr, stop());
  device.StopAndDeAllocate();
}

TEST(file_video_capture_device, start_starts_loop_with_correct_duration) {
  FakeFileVideoCaptureDevice device;
  const VideoCaptureFormat format{{}, 30.0f, {}};
  auto timedLoopPtr = new FakeTimedLoop;
  auto captureLoopClient = std::make_unique<MockCaptureLoopClient>(&device);
  auto fakeVideoCaptureDeviceClient = std::make_unique<FakeVideoCaptureDeviceClient>();

  using ::testing::_;
  EXPECT_CALL(*captureLoopClient, callback());
  device.Start(format, fakeVideoCaptureDeviceClient.get(),
               std::move(captureLoopClient), std::make_unique<MockVideoFileParser>(),
               std::unique_ptr<FakeTimedLoop>(timedLoopPtr));
  EXPECT_EQ(timedLoopPtr->lastLoopDuration_.count(), 33);
}

TEST(file_video_capture_device, on_capture_task_requests_next_frame) {
  FakeFileVideoCaptureDevice device;
  auto fileParserPtr = new MockVideoFileParser;
  auto fakeVideoCaptureDeviceClient = std::make_unique<FakeVideoCaptureDeviceClient>();

  using ::testing::_;
  EXPECT_CALL(*fileParserPtr, GetNextFrame(_));
  device.Start(VideoCaptureFormat{}, fakeVideoCaptureDeviceClient.get(),
               std::make_unique<FileVideoCaptureDevice::CaptureLoopClient>(&device),
               std::unique_ptr<MockVideoFileParser>(fileParserPtr),
               std::make_unique<FakeTimedLoop>());
}

TEST(file_video_capture_device, on_capture_task_calls_on_incoming_data_with_file_parser_format) {
  FakeFileVideoCaptureDevice device;
  VideoCaptureFormat format(Size(2, 2), 30.0f, VideoPixelFormat::UYVY);

  auto videoCaptureClient = std::make_unique<FakeVideoCaptureDeviceClient>();
  auto videoCaptureClientPtr = videoCaptureClient.get();

  using ::testing::_;
  device.Start(format, videoCaptureClient.get(),
               std::make_unique<FileVideoCaptureDevice::CaptureLoopClient>(&device),
               std::make_unique<FakeVideoFileParser>(format), std::make_unique<FakeTimedLoop>());
  EXPECT_EQ(videoCaptureClientPtr->lastFormat_, format);
}
