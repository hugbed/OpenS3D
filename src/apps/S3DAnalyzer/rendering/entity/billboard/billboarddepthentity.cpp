#include "billboarddepthentity.h"

#include <s3d/disparity/viewer_depth_converter.h>

#include <cassert>

using s3d::ViewerContext;
using s3d::ViewerDepthConverter;

BillboardDepthEntity::BillboardDepthEntity(const QSize& imageSize)
    : BillboardDisparityEntity(imageSize) {}

void BillboardDepthEntity::setDisplayRange(float /*minX*/, float /*maxX*/, float minY, float maxY) {
  float pixelToWorldRatio = maxY - minY;
  float values[] = {2.0f * m_displayZoom / pixelToWorldRatio,
                    0.0f,
                    0.0f,
                    0.0f,
                    2.0f * m_displayZoom / pixelToWorldRatio,
                    -2.0f * minY / pixelToWorldRatio - 1.0f,
                    0.0f,
                    0.0f,
                    1.0f};
  setPointToScreen(QMatrix3x3(values));
}

void BillboardDepthEntity::addShaders() {
  // todo: better name for this class and for the shaders
  m_program->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                     ":/shaders/billboardgradientworld.vert");
  m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/color.frag");
}

void BillboardDepthEntity::setHorizontalShift(float shift) {
  m_horizontalShift = shift;
  setPoints(m_points, m_intensities);
  m_program->bind();
  { m_program->setUniformValue("uHorizontalShift", shift); }
  m_program->release();
}

std::vector<Vertex> BillboardDepthEntity::verticesFromPoints(
    const std::vector<Eigen::Vector2f>& points,
    const std::vector<float>& intensities) {
  assert(points.size() == intensities.size());
  if (viewerContext == nullptr) {
    return {};
  }

  m_points = points;
  m_intensities = intensities;

  ViewerDepthConverter converter(viewerContext);
  std::vector<Vertex> vertices(points.size());
  for (auto i = 0ULL; i < points.size(); ++i) {
    const auto& position = converter.computeHorizontalPosition(points[i].x());
    auto intensity = intensities[i];
    auto d = converter.computePerceivedDepth(intensity + m_horizontalShift * 100);
    vertices[i] = Vertex(QVector3D(position, d, 1.0f), intensity);
  }
  return vertices;
}

void BillboardDepthEntity::setViewerContext(ViewerContext* context) {
  viewerContext = context;
}

void BillboardDepthEntity::setDisplayZoom(float displayZoom) {
  m_displayZoom = displayZoom;
}
