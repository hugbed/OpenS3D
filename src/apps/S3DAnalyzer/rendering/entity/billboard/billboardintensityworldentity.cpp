#include "billboardintensityworldentity.h"

BillboardIntensityWorldEntity::BillboardIntensityWorldEntity(const QSize &imageSize) :
        BillboardIntensityEntity(imageSize) {}

void BillboardIntensityWorldEntity::setDisplayRange(float minX, float maxX, float minY, float maxY) {
  float w = maxX - minX;
  float h = maxY - minY;
  float values[] = {2.0f / w, 0.0f, -1.0f, 0.0f, 2.0f/h, -2.0f* minY / h - 1.0f, 0.0f, 0.0f, 1.0f};
  setPointToScreen(QMatrix3x3(values));
}

void BillboardIntensityWorldEntity::addShaders() {
  // todo: better name for this class and for the shaders
  m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/billboardgradientworld.vert");
  m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/color.frag");
}

void BillboardIntensityWorldEntity::setHorizontalShift(float shift) {
  m_horizontalShift = shift;
  setPoints(m_points, m_intensities);
  m_program->bind();
  { m_program->setUniformValue("uHorizontalShift",  shift); }
  m_program->release();
}

std::vector<Vertex> BillboardIntensityWorldEntity::verticesFromPoints(const std::vector<QVector2D>& points,
                                       const std::vector<float>& intensities) {
  assert(points.size() == intensities.size());
  if (viewerContext == nullptr) {return {};}

  m_points = points;
  m_intensities = intensities;

  ViewerDepthConverter converter(viewerContext);
  std::vector<Vertex> vertices(points.size());
  for (auto i = 0ULL; i < points.size(); ++i) {
    const auto& point = points[i];
    auto intensity = intensities[i];
    auto d = converter.computePerceivedDepth(intensity + m_horizontalShift*100);
    vertices[i] = Vertex(QVector3D(point.x(), d, 1.0f), intensity);
  }
  return vertices;
}

void BillboardIntensityWorldEntity::setViewerContext(ViewerContext* context) {
  viewerContext = context;
}
