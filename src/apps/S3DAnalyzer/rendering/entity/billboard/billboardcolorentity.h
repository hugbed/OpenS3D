#ifndef RENDERING_ENTITY_BILLBOARD_BILLBOARDCOLORENTITY_H
#define RENDERING_ENTITY_BILLBOARD_BILLBOARDCOLORENTITY_H

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

#endif  // RENDERING_ENTITY_BILLBOARD_BILLBOARDCOLORENTITY_H
