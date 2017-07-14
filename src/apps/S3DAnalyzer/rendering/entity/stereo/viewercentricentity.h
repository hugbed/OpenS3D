#ifndef RENDERING_ENTITY_STEREO_VIEWERCENTRICENTITY_H
#define RENDERING_ENTITY_STEREO_VIEWERCENTRICENTITY_H

#include "stereoimageentity.h"

#include <s3d/disparity/viewer_depth_converter.h>

#include <gsl/gsl>

class QPaintEvent;
class QPainter;
class ViewerContext;

class ViewerCentricEntity : public StereoImageEntity {
 public:
  ViewerCentricEntity();

  void init() override;
  void setHorizontalShift(float shift) override;
  void setAspectRatio(float ratio) override;
  void setTextureLeft(QOpenGLTexture* texture) override;
  void setTextureRight(QOpenGLTexture* texture) override;
  void setViewerContext(gsl::not_null<ViewerContext*> viewerContext);
  void setDepthRangeMeters(float min, float max);

  void draw(QPaintDevice* paintDevice) override;

  QPoint worldToWidget(QPointF pos, int deviceWidth, int deviceHeight);
  float worldYToWidgetX(float y, int deviceWidth);
  float worldZToWidgetY(float z, int deviceHeight);

 private:
  void drawScreen(QPainter* painter);
  void drawViewer(QPainter* painter);
  void drawPoints(QPainter* painter) {}

 private:
  ViewerContext* m_viewerContext{};

  float m_minZ{-1.0f};
  float m_maxZ{3.0f};
};

#endif  // RENDERING_ENTITY_STEREO_VIEWERCENTRICENTITY_H
