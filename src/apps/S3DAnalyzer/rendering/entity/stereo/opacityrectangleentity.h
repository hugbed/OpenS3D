#ifndef S3D_ANALYZER_RENDERING_ENTITY_STEREO_OPACITYRECTANGLEENTITY_H
#define S3D_ANALYZER_RENDERING_ENTITY_STEREO_OPACITYRECTANGLEENTITY_H

#include "rectangleentity.h"

class OpacityRectangleEntity : public RectangleEntity {
 public:
  OpacityRectangleEntity();
  void addShaders() override;
};

#endif  // S3D_ANALYZER_RENDERING_ENTITY_STEREO_ANAGLYPHRECTANGLEENTITY_H
