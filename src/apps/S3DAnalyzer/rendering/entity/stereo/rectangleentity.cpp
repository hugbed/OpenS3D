#include "rectangleentity.h"

#include <cassert>

RectangleEntity::RectangleEntity() {
  initializeOpenGLFunctions();
}

void RectangleEntity::init() {
  init(fullscreenVertices());
}

void RectangleEntity::init(float left, float top, float right, float bottom) {
  init(cornersVertices(left, top, right, bottom));
}

void RectangleEntity::init(const std::vector<Vertex>& vertices) {
  createShader();
  createBuffer(vertices);
  createVertexArrayObject();
  releaseAll();
}

void RectangleEntity::createShader() {
  m_program = std::make_unique<QOpenGLShaderProgram>();
  addShaders();
  m_program->link();
  m_program->bind();
}

void RectangleEntity::createBuffer(const std::vector<Vertex>& vertices) {
  m_vertex.create();
  m_vertex.bind();
  m_vertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_vertex.allocate(&vertices[0], vertices.size() * sizeof(Vertex));
}

void RectangleEntity::createVertexArrayObject() {
  m_object.create();
  m_object.bind();
  m_program->bind();
  m_program->enableAttributeArray(0);
  m_program->enableAttributeArray(1);
  m_program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize,
                                Vertex::stride());
  m_program->setAttributeBuffer(1, GL_FLOAT, Vertex::texCoordOffset(), Vertex::TexCoordTupleSize,
                                Vertex::stride());
  setDefaultUniforms();
}

void RectangleEntity::releaseAll() {
  m_object.release();
  m_vertex.release();
  m_program->release();
}

RectangleEntity::~RectangleEntity() {
  m_object.destroy();
  m_vertex.destroy();
  m_program.reset();
}

void RectangleEntity::setTexture(int index, QOpenGLTexture* texture) {
  auto it = m_textures.find(index);

  // replace if already in map
  if (it != m_textures.end()) {
    it->second = texture;
  } else {  // add it to the map
    m_textures.insert(std::pair<int, QOpenGLTexture*>(index, texture));
  }
}

void RectangleEntity::setHorizontalShift(float shift) {
  m_program->bind();
  { m_program->setUniformValue("uHorizontalShift", shift); }
  m_program->release();
}

void RectangleEntity::setTextureLeft(QOpenGLTexture* texture) {
  setTexture(0, texture);
}

void RectangleEntity::setTextureRight(QOpenGLTexture* texture) {
  setTexture(1, texture);
}

void RectangleEntity::setAspectRatio(float ratio) {
  m_program->bind();
  { m_program->setUniformValue("uAspectRatio", ratio); }
  m_program->release();
}

void RectangleEntity::setCorners(float left, float top, float right, float bottom) {
  m_vertex.bind();
  {
    m_object.bind();
    {
      auto vertices = cornersVertices(left, top, right, bottom);
      m_vertex.write(0, &vertices[0], vertices.size() * sizeof(Vertex));
    }
    m_object.release();
  }
  m_vertex.release();
}

void RectangleEntity::draw() {
  // Render using our shader
  m_program->bind();
  {
    for (auto textureIt = std::begin(m_textures); textureIt != std::end(m_textures); ++textureIt) {
      glActiveTexture(static_cast<GLenum>((static_cast<int>(GL_TEXTURE0) + (*textureIt).first)));
      (*textureIt).second->bind();
    }
    m_object.bind();
    { glDrawArrays(GL_TRIANGLES, 0, NB_VERTICES); }
    m_object.release();
  }
  m_program->release();
}

void RectangleEntity::addShaders() {
  m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/textureshift.vert");
  m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
}

void RectangleEntity::setDefaultUniforms() {
  // set possible uniforms (mono/stereo textures)
  // possible because of early return for non-existent uniforms in setUniformValue
  m_program->bind();
  {
    m_program->setUniformValue("uTexture0", 0);
    m_program->setUniformValue("uTexture1", 1);
    setHorizontalShift(0.0f);
    setAspectRatio(1.0f);
  }
  m_program->release();
}

// static
std::vector<Vertex> RectangleEntity::fullscreenVertices() {
  constexpr float left = -1.0f;
  constexpr float top = 1.0f;
  constexpr float right = 1.0f;
  constexpr float bottom = -1.0f;
  return cornersVertices(left, top, right, bottom);
}

// static
std::vector<Vertex> RectangleEntity::cornersVertices(float left,
                                                     float top,
                                                     float right,
                                                     float bottom) {
  return {{QVector3D(left, top, 1.0f), QVector2D(0.0f, 1.0f)},
          {QVector3D(right, top, 1.0f), QVector2D(1.0f, 1.0f)},
          {QVector3D(right, bottom, 1.0f), QVector2D(1.0f, 0.0f)},
          {QVector3D(left, top, 1.0f), QVector2D(0.0f, 1.0f)},
          {QVector3D(right, bottom, 1.0f), QVector2D(1.0f, 0.0f)},
          {QVector3D(left, bottom, 1.0f), QVector2D(0.0f, 0.0f)}};
}
