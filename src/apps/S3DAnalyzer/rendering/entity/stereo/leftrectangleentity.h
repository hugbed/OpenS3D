#ifndef S3D_ANALYZER_RENDERING_ENTITY_STEREO_LEFTRECTANGLEENTITY_H
#define S3D_ANALYZER_RENDERING_ENTITY_STEREO_LEFTRECTANGLEENTITY_H

#include "rectangleentity.h"

class LeftRectangleEntity : public RectangleEntity {
 public:
  LeftRectangleEntity();
  void setTextureRight(QOpenGLTexture* /*texture*/) override;
  void setHorizontalShift(float shift) override;
};

#endif  // S3D_ANALYZER_RENDERING_ENTITY_STEREO_LEFTRECTANGLEENTITY_H
