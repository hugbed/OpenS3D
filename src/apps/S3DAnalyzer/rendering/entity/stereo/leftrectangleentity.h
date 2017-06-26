#ifndef RENDERING_ENTITY_STEREO_LEFTRECTANGLEENTITY_H
#define RENDERING_ENTITY_STEREO_LEFTRECTANGLEENTITY_H

#include "rectangleentity.h"

class LeftRectangleEntity : public RectangleEntity {
 public:
  LeftRectangleEntity();
  void setTextureRight(QOpenGLTexture* /*texture*/) override;
  void setHorizontalShift(float shift) override;
};

#endif  // RENDERING_ENTITY_STEREO_LEFTRECTANGLEENTITY_H
