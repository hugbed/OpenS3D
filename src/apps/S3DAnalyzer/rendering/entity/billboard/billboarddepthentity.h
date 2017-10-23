#ifndef RENDERING_ENTITY_BILLBOARD_BILLBOARDDEPTHENTITY_H
#define RENDERING_ENTITY_BILLBOARD_BILLBOARDDEPTHENTITY_H

#include "billboarddisparityentity.h"

namespace s3d {
struct ViewerContext;
}

// todo: review names (difference: this does not have aspect ratio)
class BillboardDepthEntity : public BillboardDisparityEntity {
 public:
  explicit BillboardDepthEntity(const QSize& imageSize);

  void setDisplayRange(float minX, float maxX, float minY, float maxY) override;
  void addShaders() override;
  void setHorizontalShift(float shift) override;
  std::vector<Vertex> verticesFromPoints(const std::vector<Eigen::Vector2f>& points,
                                         const std::vector<float>& intensities) override;
  void setViewerContext(s3d::ViewerContext* context);
  void setDisplayZoom(float displayZoom);

 private:
  float m_displayZoom{1.0f};
  std::vector<Eigen::Vector2f> m_points;
  std::vector<float> m_intensities;
  float m_horizontalShift{0.0};
  s3d::ViewerContext* viewerContext{};
};

#endif  // RENDERING_ENTITY_BILLBOARD_BILLBOARDDEPTHENTITY_H
