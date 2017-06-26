#include "billboardintensityentity.h"

#include <cassert>

BillboardIntensityEntity::BillboardIntensityEntity(const QSize& imageSize)
    : BillboardEntity(imageSize) {}

void BillboardIntensityEntity::setDefaultAttributeBuffers() {
  m_program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize,
                                Vertex::stride());
  m_program->setAttributeBuffer(1, GL_FLOAT, Vertex::intensityOffset(), Vertex::IntensitySize,
                                Vertex::stride());
}

void BillboardIntensityEntity::setPoints(const std::vector<QVector2D>& points,
                                         const std::vector<float>& intensities) {
  if (!points.empty() && points.size() == intensities.size()) {
    m_vertex.bind();
    {
      m_object.bind();
      {
        m_vertices.clear();
        m_vertices = verticesFromPoints(points, intensities);
        m_vertex.allocate(&m_vertices[0], m_vertices.size() * sizeof(Vertex));
      }
      m_object.release();
    }
    m_vertex.release();
  }
}

void BillboardIntensityEntity::setDefaultUniforms() {
  BillboardEntity::setDefaultUniforms();
  setMinIntensity(0.0f);
  setMaxIntensity(1.0f);
}

void BillboardIntensityEntity::setMinIntensity(float value) {
  m_program->bind();
  { m_program->setUniformValue("uMinIntensity", value); }
  m_program->release();
}

void BillboardIntensityEntity::setMaxIntensity(float value) {
  m_program->bind();
  { m_program->setUniformValue("uMaxIntensity", value); }
  m_program->release();
}

std::vector<Vertex> BillboardIntensityEntity::verticesFromPoints(
    const std::vector<QVector2D>& points,
    const std::vector<float>& intensities) {
  assert(points.size() == intensities.size());

  std::vector<Vertex> vertices(points.size());
  for (auto i = 0ULL; i < points.size(); ++i) {
    const auto& point = points[i];
    auto intensity = intensities[i];

    vertices[i] = Vertex(QVector3D(point.x(), point.y(), 1.0f), intensity);
  }

  return vertices;
}

void BillboardIntensityEntity::addShaders() {
  m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/billboardgradient.vert");
  m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/color.frag");
}
