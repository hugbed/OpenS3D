#ifndef S3D_ANALYZER_RENDERING_ENTITY_STEREO_STEREOIMAGEENTITY_H
#define S3D_ANALYZER_RENDERING_ENTITY_STEREO_STEREOIMAGEENTITY_H

#include "rendering/entity/entity.h"

#include <QOpenGLTexture>

class StereoImageEntity : public Entity {
 public:
  virtual void init() = 0;
  virtual void setHorizontalShift(float shift) = 0;
  virtual void setAspectRatio(float ratio) = 0;
  virtual void setTextureLeft(QOpenGLTexture* texture) = 0;
  virtual void setTextureRight(QOpenGLTexture* texture) = 0;
};

#endif  // S3D_ANALYZER_RENDERING_ENTITY_STEREO_STEREOIMAGEENTITY_H
