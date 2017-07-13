#include "s3d/disparity/viewer_depth_converter.h"

using Point = ViewerDepthConverter::Point;
using Pointf = ViewerDepthConverter::Pointf;

ViewerDepthConverter::ViewerDepthConverter(gsl::not_null<ViewerContext*> context)
    : viewerContext_(context) {}

std::vector<float> ViewerDepthConverter::computePerceivedDepth(
    const std::vector<float>& disparities) {  // todo: in percent, S_r = screenWidth
  std::vector<float> perceivedDepth_;
  perceivedDepth_.reserve(perceivedDepth_.size());

  for (int i = 0; i < disparities.size(); ++i) {
    perceivedDepth_.emplace_back(computePerceivedDepth(disparities[i]));
  }
  return perceivedDepth_;
}

// horizontal position and depth in meters
std::vector<Pointf> ViewerDepthConverter::computeDepthPositions(
    const std::vector<Point>& imagePoints,
    const std::vector<float>& disparities) {
  auto perceivedDepth = computePerceivedDepth(disparities);
  auto horizontalPos = computeHorizontalPositions(imagePoints);
  assert(perceivedDepth.size() == horizontalPos.size());

  std::vector<Pointf> depthPositions;
  for (int i = 0; i < perceivedDepth.size(); ++i) {
    depthPositions.emplace_back(horizontalPos[i], perceivedDepth[i]);
  }

  return depthPositions;
}

float ViewerDepthConverter::  computePerceivedDepth(float disparityPercent) {
  auto&& Z_0_e = viewerContext_->viewerDistance;
  auto&& b_e = viewerContext_->interocularDistance;
  // for disparities in percent
  float S_r = viewerContext_->screenWidth;// / static_cast<float>(viewerContext_->imageWidthPixels);
  float d_s = S_r * disparityPercent / 100.0f;
  return Z_0_e - (Z_0_e * b_e) / (b_e - d_s);
}

std::vector<float> ViewerDepthConverter::computeHorizontalPositions(
    const std::vector<Point>& imagePoints) {
  std::vector<float> horizontalPos_;
  horizontalPos_.reserve(imagePoints.size());

  // todo: set imageWidthPixels when changing image or video
  float S_r = viewerContext_->screenWidth / static_cast<float>(viewerContext_->imageWidthPixels);

  // fill horizontal positions from screen ratio
  for (auto&& imagePoint : imagePoints) {
    horizontalPos_.emplace_back(S_r * static_cast<float>(imagePoint.x()));
  }

  return horizontalPos_;
}

void ViewerDepthConverter::setViewerContext(gsl::not_null<ViewerContext*> viewerContext) {
  viewerContext_ = viewerContext;
}
