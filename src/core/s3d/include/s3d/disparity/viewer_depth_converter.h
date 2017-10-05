#ifndef S3D_DISPARITY_VIEWER_DEPTH_CONVERTER_H
#define S3D_DISPARITY_VIEWER_DEPTH_CONVERTER_H

#include <Eigen/Dense>

#include <vector>

#include <gsl/gsl>

namespace s3d {

struct ViewerContext;

class ViewerDepthConverter {
 public:
  using Point = Eigen::Vector2d;
  using Pointf = Eigen::Vector2f;

  explicit ViewerDepthConverter(gsl::not_null<ViewerContext*> context);

  std::vector<float> computePerceivedDepth(const std::vector<float>& disparitiesPercent);

  // horizontal position and depth in meters
  std::vector<Pointf> computeDepthPositions(const std::vector<Point>& imagePoints,
                                            const std::vector<float>& disparities);

  float computePerceivedDepth(float disparityPercent);

  // 0 in the middle of the image
  float computeHorizontalPosition(float imageX);

  std::vector<float> computeHorizontalPositions(const std::vector<Point>& imagePoints);

  void setViewerContext(gsl::not_null<ViewerContext*> viewerContext);

 private:
  ViewerContext* viewerContext_;
};
}  // namespace s3d

#endif  // S3D_DISPARITY_VIEWER_DEPTH_CONVERTER_H
