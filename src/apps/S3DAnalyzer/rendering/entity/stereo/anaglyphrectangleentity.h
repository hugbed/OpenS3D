#ifndef S3D_ANALYZER_RENDERING_ENTITY_STEREO_ANAGLYPHRECTANGLEENTITY_H
#define S3D_ANALYZER_RENDERING_ENTITY_STEREO_ANAGLYPHRECTANGLEENTITY_H

#include "rectangleentity.h"

class AnaglyphRectangleEntity : public RectangleEntity {
 public:
  AnaglyphRectangleEntity();
  void addShaders() override;
};

#endif  // S3D_ANALYZER_RENDERING_ENTITY_STEREO_ANAGLYPHRECTANGLEENTITY_H
