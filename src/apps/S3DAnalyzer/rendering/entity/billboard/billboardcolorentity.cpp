#include "billboardcolorentity.h"

#include <QColor>

#include <cassert>

BillboardColorEntity::BillboardColorEntity(const QSize& imageSize) : BillboardEntity(imageSize) {}

std::vector<Vertex> BillboardColorEntity::verticesFromPoints(const std::vector<QVector2D>& points,
                                                             const std::vector<QColor>& colors) {
  assert(points.size() == colors.size());

  std::vector<Vertex> vertices(points.size());
  for (auto i = 0ULL; i < points.size(); ++i) {
    const auto& color = colors[i];
    const auto& point = points[i];

    vertices[i] = Vertex(QVector3D(point.x(), point.y(), 1.0f),
                         QVector3D(color.redF(), color.greenF(), color.blueF()));
  }

  return vertices;
}

void BillboardColorEntity::setPoints(const std::vector<QVector2D>& points,
                                     const std::vector<QColor>& colors) {
  m_vertex.bind();
  {
    m_object.bind();
    {
      m_vertices = verticesFromPoints(points, colors);
      m_vertex.allocate(&m_vertices[0], m_vertices.size() * sizeof(Vertex));
    }
    m_object.release();
  }
  m_vertex.release();
}

void BillboardColorEntity::setDefaultAttributeBuffers() {
  m_program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize,
                                Vertex::stride());
  m_program->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize,
                                Vertex::stride());
}

void BillboardColorEntity::addShaders() {
  m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/billboard.vert");
  m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/color.frag");
}
