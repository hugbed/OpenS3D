#ifndef S3D_ANALYZER_RENDERING_ENTITY_STEREO_INTERLACEDRECTANGLEENTITY_H
#define S3D_ANALYZER_RENDERING_ENTITY_STEREO_INTERLACEDRECTANGLEENTITY_H

#include "rectangleentity.h"

class InterlacedRectangleEntity : public RectangleEntity {
 public:
  InterlacedRectangleEntity();
  void addShaders() override;
};

#endif  // S3D_ANALYZER_RENDERING_ENTITY_STEREO_INTERLACEDRECTANGLEENTITY_H
