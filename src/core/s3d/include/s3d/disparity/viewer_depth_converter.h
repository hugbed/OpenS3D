#ifndef S3D_DISPARITY_VIEWER_DEPTH_CONVERTER_H
#define S3D_DISPARITY_VIEWER_DEPTH_CONVERTER_H

#include <Eigen/Dense>

#include <vector>

#include <gsl/gsl>

struct ViewerContext {
  ViewerContext(float viewerDistance,
                float interocularDistance,
                float screenWidth,
                int imageWidthPixels)
      : viewerDistance{viewerDistance},
        interocularDistance{interocularDistance},
        screenWidth{screenWidth},
        imageWidthPixels{imageWidthPixels} {}

  float viewerDistance;
  float interocularDistance;
  float screenWidth;
  int imageWidthPixels;
};

class ViewerDepthConverter {
 public:
  using Point = Eigen::Vector2d;
  using Pointf = Eigen::Vector2f;

  ViewerDepthConverter(gsl::not_null<ViewerContext*> context);

  std::vector<float> computePerceivedDepth(const std::vector<float>& disparitiesPercent);

  // horizontal position and depth in meters
  std::vector<Pointf> computeDepthPositions(const std::vector<Point>& imagePoints,
                                            const std::vector<float>& disparities);

  float computePerceivedDepth(float disparitiesPercent);

  std::vector<float> computeHorizontalPositions(const std::vector<Point>& imagePoints);

  void setViewerContext(gsl::not_null<ViewerContext*> viewerContext);

 private:
  ViewerContext* viewerContext_;
};

#endif  // S3D_DISPARITY_VIEWER_DEPTH_CONVERTER_H
