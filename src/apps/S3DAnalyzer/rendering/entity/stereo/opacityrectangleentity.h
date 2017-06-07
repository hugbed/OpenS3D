#ifndef RENDERING_ENTITY_STEREO_OPACITYRECTANGLEENTITY_H
#define RENDERING_ENTITY_STEREO_OPACITYRECTANGLEENTITY_H

#include "rectangleentity.h"

class OpacityRectangleEntity : public RectangleEntity {
 public:
  OpacityRectangleEntity();
  void addShaders() override;
};

#endif  // RENDERING_ENTITY_STEREO_ANAGLYPHRECTANGLEENTITY_H
