#ifndef RENDERING_ENTITY_BILLBOARD_BILLBOARDDEPTHENTITY_H
#define RENDERING_ENTITY_BILLBOARD_BILLBOARDDEPTHENTITY_H

#include "billboarddisparityentity.h"

#include <s3d/disparity/viewer_depth_converter.h>

#include <cassert>

// todo: review names (difference: this does not have aspect ratio)
class BillboardDepthEntity : public BillboardDisparityEntity {
 public:
  explicit BillboardDepthEntity(const QSize& imageSize);

  void setDisplayRange(float minX, float maxX, float minY, float maxY) override;
  void addShaders() override;
  void setHorizontalShift(float shift) override;
  std::vector<Vertex> verticesFromPoints(const std::vector<Eigen::Vector2f>& points,
                                         const std::vector<float>& intensities) override;
  void setViewerContext(ViewerContext* context);

 private:
  std::vector<Eigen::Vector2f> m_points;
  std::vector<float> m_intensities;
  float m_horizontalShift{0.0};
  ViewerContext* viewerContext{};
};

#endif  // RENDERING_ENTITY_BILLBOARD_BILLBOARDDEPTHENTITY_H
