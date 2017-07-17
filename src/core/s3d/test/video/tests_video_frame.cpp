#include "gtest/gtest.h"

#include "s3d/video/video_frame.h"

TEST(video_frame, num_bytes_per_pixel_ARGB_BGRA_4) {
  EXPECT_EQ(VideoFrame::AllocationSize(VideoPixelFormat::ARGB, Size(1, 1)), 4);
  EXPECT_EQ(VideoFrame::AllocationSize(VideoPixelFormat::BGRA, Size(1, 1)), 4);
}

TEST(video_frame, num_bytes_per_pixel_UYVY_2) {
  EXPECT_EQ(VideoFrame::AllocationSize(VideoPixelFormat::UYVY, Size(1, 1)), 2);
}

TEST(video_frame, num_bytes_per_pixel_RGB_3) {
  EXPECT_EQ(VideoFrame::AllocationSize(VideoPixelFormat::RGB, Size(1, 1)), 3);
}

TEST(video_frame, num_bytes_per_pixel_BGR_3) {
  EXPECT_EQ(VideoFrame::AllocationSize(VideoPixelFormat::RGB, Size(1, 1)), 3);
}

TEST(video_frame, num_bytes_per_pixel_UNKNOWN_0) {
  EXPECT_EQ(VideoFrame::AllocationSize(VideoPixelFormat::UNKNOWN, Size(1, 1)), 0);
}

TEST(video_frame, allocation_size_num_bytes_times_width_times_height) {
  auto format = VideoPixelFormat::ARGB;
  auto size = Size(2, 3);
  auto nbBytes = VideoFrame::AllocationSize(format, Size(1, 1));
  EXPECT_EQ(VideoFrame::AllocationSize(format, size), nbBytes * size.getWidth() * size.getHeight());
}
