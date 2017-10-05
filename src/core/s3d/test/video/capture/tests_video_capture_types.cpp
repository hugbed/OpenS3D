#include "gtest/gtest.h"

#include "s3d/video/capture/video_capture_types.h"

using s3d::Size;
using s3d::VideoCaptureFormat;
using s3d::VideoPixelFormat;

TEST(video_capture_format, default_values) {
  VideoCaptureFormat format{};

  // default frame rate != 0 since this may cause divide by zero
  EXPECT_EQ(format.frameSize, Size(0, 0));
  EXPECT_FLOAT_EQ(format.frameRate, -1.0f);
  EXPECT_EQ(format.pixelFormat, VideoPixelFormat::UNKNOWN);
  EXPECT_EQ(format.stereo3D, false);
}

TEST(video_capture_format, default_stereo_false) {
  const auto size = Size(1, 2);
  const auto frameRate = 3.0f;
  const auto pixelFormat = VideoPixelFormat::ARGB;

  VideoCaptureFormat format{size, frameRate, pixelFormat};

  EXPECT_EQ(format.frameSize, size);
  EXPECT_FLOAT_EQ(format.frameRate, frameRate);
  EXPECT_EQ(format.pixelFormat, pixelFormat);
  EXPECT_EQ(format.stereo3D, false);
}

TEST(video_capture_format, params_stereo_true) {
  VideoCaptureFormat format{{}, {}, {}, true};
  EXPECT_EQ(format.stereo3D, true);
}

TEST(video_capture_format, image_allocation_size_stereo_is_same) {
  constexpr auto size = Size(1, 2);
  constexpr auto frameRate = 3.0f;
  constexpr auto pixelFormat = VideoPixelFormat::ARGB;
  constexpr VideoCaptureFormat format2D{size, frameRate, pixelFormat};

  constexpr auto nbBytesPerPixel = 4;
  constexpr auto expectedSize = nbBytesPerPixel * size.getWidth() * size.getHeight();
  EXPECT_EQ(format2D.ImageAllocationSize(), expectedSize);

  constexpr VideoCaptureFormat format3D{
      format2D.frameSize, format2D.frameRate, format2D.pixelFormat, true};
  EXPECT_EQ(format3D.ImageAllocationSize(), expectedSize);
}
