#ifndef RENDERING_ENTITY_BILLBOARD_BILLBOARDINTENSITYENTITY_H
#define RENDERING_ENTITY_BILLBOARD_BILLBOARDINTENSITYENTITY_H

#include "billboardentity.h"

class BillboardIntensityEntity : public BillboardEntity {
 public:
  explicit BillboardIntensityEntity(const QSize& imageSize);

  void setPoints(const std::vector<QVector2D>& points, const std::vector<float>& intensities);
  void setMinIntensity(float value);
  void setMaxIntensity(float value);

  void addShaders() override;
  void setDefaultAttributeBuffers() override;
  void setDefaultUniforms() override;

 private:
  std::vector<Vertex> verticesFromPoints(const std::vector<QVector2D>& points,
                                         const std::vector<float>& intensities);
};

#endif  // RENDERING_ENTITY_BILLBOARD_BILLBOARDINTENSITYENTITY_H
