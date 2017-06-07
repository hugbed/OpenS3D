#ifndef S3D_ANALYZER_RENDERING_ENTITY_BILLBOARD_BILLBOARDCOLORENTITY_H
#define S3D_ANALYZER_RENDERING_ENTITY_BILLBOARD_BILLBOARDCOLORENTITY_H

#include "billboardentity.h"

class BillboardColorEntity : public BillboardEntity {
 public:
  explicit BillboardColorEntity(const QSize& size);

  void setPoints(const std::vector<QVector2D>& points, const std::vector<QColor>& colors);

 private:
  void addShaders() override;
  void setDefaultAttributeBuffers() override;

  std::vector<Vertex> verticesFromPoints(const std::vector<QVector2D>& points,
                                         const std::vector<QColor>& colors);
};

#endif  // S3D_ANALYZER_RENDERING_ENTITY_BILLBOARD_BILLBOARDCOLORENTITY_H
