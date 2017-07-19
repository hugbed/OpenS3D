#ifndef RENDERING_ENTITY_STEREO_STEREOSIDEBYSIDEENTITY_H
#define RENDERING_ENTITY_STEREO_STEREOSIDEBYSIDEENTITY_H

#include "stereoimageentity.h"

#include "rectangleentity.h"

// could have a flag keep aspect ratio (true or false)
// horizontally squished images are useful for side-by-side 3D monitors

class StereoSideBySideEntity : public StereoImageEntity {
 public:
  StereoSideBySideEntity();
  explicit StereoSideBySideEntity(bool halfWidthResolution);

  void init() override;
  void setTextureLeft(QOpenGLTexture* texture) override;
  void setTextureRight(QOpenGLTexture* texture) override;
  void draw(QPaintDevice* parent) override;
  void setHorizontalShift(float shift) override;
  void setAspectRatio(float ratio) override;

 private:
  RectangleEntity m_rectangles[2];
  bool m_halfWidthResolution{false};
};

#endif  // RENDERING_ENTITY_STEREO_STEREOSIDEBYSIDEENTITY_H
