#include "viewercentricentity.h"

#include "utilities/usersettings.h"

#include <QPaintEvent>
#include <QPainter>

using s3d::ViewerContext;

ViewerCentricEntity::ViewerCentricEntity() = default;

void ViewerCentricEntity::init() {}
void ViewerCentricEntity::setHorizontalShift(float /*shift*/) {}
void ViewerCentricEntity::setAspectRatio(float /*ratio*/) {}
void ViewerCentricEntity::setTextureLeft(QOpenGLTexture* /*texture*/) {}
void ViewerCentricEntity::setTextureRight(QOpenGLTexture* /*texture*/) {}

void ViewerCentricEntity::setViewerContext(gsl::not_null<ViewerContext*> viewerContext) {
  m_viewerContext = viewerContext;
}

void ViewerCentricEntity::setDepthRangeMeters(float min, float max) {
  m_minZ = min;
  m_maxZ = max;
}

void ViewerCentricEntity::draw(QPaintDevice* paintDevice) {
  if (paintDevice == nullptr || m_viewerContext == nullptr) {
    return;
  }
  QPainter painter(paintDevice);
  drawScreen(&painter);
  drawViewer(&painter);
}

QPoint ViewerCentricEntity::worldToWidget(QPointF pos, int deviceWidth, int deviceHeight) {
  if (m_viewerContext == nullptr) {
    return {};
  }

  auto y = static_cast<float>(pos.x());
  auto z = static_cast<float>(pos.y());
  return {static_cast<int>(worldYToWidgetX(y, deviceWidth, deviceHeight)),
          static_cast<int>(worldZToWidgetY(z, deviceHeight))};
}

float ViewerCentricEntity::worldYToWidgetX(float y, int deviceWidth, int deviceHeight) {
  return y * m_displayZoom * getPixelToWorldRatio(deviceHeight) + deviceWidth / 2;
}

float ViewerCentricEntity::worldZToWidgetY(float z, int deviceHeight) {
  float widgetY = (z - m_minZ) * getPixelToWorldRatio(deviceHeight);
  auto mid = static_cast<float>(deviceHeight) / 2.0f;
  widgetY = (widgetY - mid) * m_displayZoom + mid;
  return widgetY;
}

float ViewerCentricEntity::getPixelToWorldRatio(float deviceHeight) {
  return deviceHeight / (m_maxZ - m_minZ);
}

void ViewerCentricEntity::drawScreen(QPainter* painter) {
  float halfScreenWidth = m_viewerContext->screenWidth;

  QPoint rectMidLeft =
      worldToWidget({-halfScreenWidth, 0}, painter->device()->width(), painter->device()->height());
  QPoint rectMidRight =
      worldToWidget({halfScreenWidth, 0}, painter->device()->width(), painter->device()->height());

  painter->drawRect(QRect(rectMidLeft + QPoint(0, 5), rectMidRight + QPoint(0, -5)));
}

void ViewerCentricEntity::drawViewer(QPainter* painter) {
  float leftEyeY = -m_viewerContext->interocularDistance / 2.0f;
  float rightEyeY = leftEyeY + m_viewerContext->interocularDistance;
  float eyeZ = m_viewerContext->viewerDistance;

  QPoint leftEyePos =
      worldToWidget({leftEyeY, eyeZ}, painter->device()->width(), painter->device()->height());
  QPoint rightEyePos =
      worldToWidget({rightEyeY, eyeZ}, painter->device()->width(), painter->device()->height());

  painter->drawEllipse(leftEyePos, 5, 5);
  painter->drawEllipse(rightEyePos, 5, 5);
}

void ViewerCentricEntity::setDisplayZoom(float displayZoom) {
  m_displayZoom = displayZoom;
}
