#include "gtest/gtest.h"

#include "s3d/disparity/viewer_depth_converter.h"
#include "s3d/disparity/viewer_context.h"

TEST(viewer_depth_converter, perceived_depth_correct) {
  ViewerContext context(2.0, 0.05, 1.0, 1920);

  std::vector<float> disparities = {20};

  ViewerDepthConverter converter(&context);
  auto perceivedDepth = converter.computePerceivedDepth(disparities);
  EXPECT_EQ(perceivedDepth.size(), 1);
  EXPECT_NEAR(perceivedDepth[0], 2.66f, 0.01f);
}

TEST(viewer_depth_converter, horizontal_position_half_pixels_is_half_screen_width) {
  constexpr float screenWidth = 1.0f;
  constexpr int imgWidth = 1920;
  ViewerContext context(2.0, 0.05, screenWidth, imgWidth);

  std::vector<Eigen::Vector2d> imagePoints = {Eigen::Vector2d{imgWidth / 2, 2}};

  ViewerDepthConverter converter(&context);
  auto positions = converter.computeHorizontalPositions(imagePoints);
  EXPECT_EQ(positions.size(), 1);
  EXPECT_NEAR(positions[0], screenWidth / 2.0f, 0.02f);
}

TEST(viewer_depth_converter, depth_positions_correct) {
  constexpr float screenWidth = 1.0f;
  constexpr int imgWidth = 1920;
  ViewerContext context(2.0, 0.05, screenWidth, imgWidth);

  std::vector<Eigen::Vector2d> imagePoints = {Eigen::Vector2d{imgWidth / 2, 2}};
  std::vector<float> disparities = {20};

  ViewerDepthConverter converter(&context);
  converter.setViewerContext(&context);  // test set context
  auto positions = converter.computeDepthPositions(imagePoints, disparities);
  EXPECT_EQ(positions.size(), 1);
  EXPECT_NEAR(positions[0].x(), screenWidth / 2.0f, 0.01f);
  EXPECT_NEAR(positions[0].y(), 2.66f, 0.01f);
}
