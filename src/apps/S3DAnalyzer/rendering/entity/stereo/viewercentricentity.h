#ifndef RENDERING_ENTITY_STEREO_VIEWERCENTRICENTITY_H
#define RENDERING_ENTITY_STEREO_VIEWERCENTRICENTITY_H

#include "stereoimageentity.h"

#include <gsl/gsl>

class QPaintEvent;
class QPainter;

namespace s3d {
struct ViewerContext;
}  // namespace s3d

class ViewerCentricEntity : public StereoImageEntity {
 public:
  ViewerCentricEntity();

  void init() override;
  void setHorizontalShift(float shift) override;
  void setAspectRatio(float ratio) override;
  void setTextureLeft(QOpenGLTexture* texture) override;
  void setTextureRight(QOpenGLTexture* texture) override;
  void setViewerContext(gsl::not_null<s3d::ViewerContext*> viewerContext);
  void setDepthRangeMeters(float min, float max);
  void setDisplayZoom(float displayZoom);

  void draw(QPaintDevice* paintDevice) override;

  QPoint worldToWidget(QPointF pos, int deviceWidth, int deviceHeight);
  float worldYToWidgetX(float y, int deviceWidth, int deviceHeight);
  float worldZToWidgetY(float z, int deviceHeight);
  float getPixelToWorldRatio(float deviceHeight);

 private:
  void drawScreen(QPainter* painter);
  void drawViewer(QPainter* painter);

 private:
  s3d::ViewerContext* m_viewerContext{};
  float m_displayZoom{1.0f};

  float m_minZ{-1.0f};
  float m_maxZ{3.0f};
};

#endif  // RENDERING_ENTITY_STEREO_VIEWERCENTRICENTITY_H
