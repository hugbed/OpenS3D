#include <utilities/usersettings.h>
#include "viewercentricentity.h"

#include <QPaintEvent>
#include <QPainter>

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
  drawPoints(&painter);
}

QPoint ViewerCentricEntity::worldToWidget(QPointF pos, int deviceWidth, int deviceHeight) {
  if (m_viewerContext == nullptr) {
    return {};
  }

  float y = static_cast<float>(pos.x());
  float z = static_cast<float>(pos.y());
  return {static_cast<int>(worldYToWidgetX(y, deviceWidth)),
          static_cast<int>(worldZToWidgetY(z, deviceHeight))};
}

float ViewerCentricEntity::worldYToWidgetX(float y, int deviceWidth) {
  float widthRatio =
      static_cast<float>(deviceWidth) / static_cast<float>(m_viewerContext->screenWidth);
  return y * widthRatio;
}

float ViewerCentricEntity::worldZToWidgetY(float z, int deviceHeight) {
  float heightRatio = static_cast<float>(deviceHeight) / (m_maxZ - m_minZ);
  return (z - m_minZ) * heightRatio;
}

void ViewerCentricEntity::drawScreen(QPainter* painter) {
  QPoint rectTopLeft =
      worldToWidget({0, 0}, painter->device()->width(), painter->device()->height());

  painter->drawRect(
      QRect(rectTopLeft + QPoint(0, 5), rectTopLeft + QPoint(painter->device()->width(), -5)));
}

void ViewerCentricEntity::drawViewer(QPainter* painter) {
  float leftEyeY =
      m_viewerContext->screenWidth / 2.0f - m_viewerContext->interocularDistance / 2.0f;
  float rightEyeY = leftEyeY + m_viewerContext->interocularDistance;
  float eyeZ = m_viewerContext->viewerDistance;

  QPoint leftEyePos =
      worldToWidget({leftEyeY, eyeZ}, painter->device()->width(), painter->device()->height());
  QPoint rightEyePos =
      worldToWidget({rightEyeY, eyeZ}, painter->device()->width(), painter->device()->height());

  painter->drawEllipse(leftEyePos, 5, 5);
  painter->drawEllipse(rightEyePos, 5, 5);
}
