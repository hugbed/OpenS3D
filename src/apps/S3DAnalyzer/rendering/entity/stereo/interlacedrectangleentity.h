#ifndef RENDERING_ENTITY_STEREO_INTERLACEDRECTANGLEENTITY_H
#define RENDERING_ENTITY_STEREO_INTERLACEDRECTANGLEENTITY_H

#include "rectangleentity.h"

class InterlacedRectangleEntity : public RectangleEntity {
 public:
  InterlacedRectangleEntity();
  void addShaders() override;
};

#endif  // RENDERING_ENTITY_STEREO_INTERLACEDRECTANGLEENTITY_H
