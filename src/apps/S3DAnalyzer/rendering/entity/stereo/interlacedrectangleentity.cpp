#include "interlacedrectangleentity.h"

InterlacedRectangleEntity::InterlacedRectangleEntity() : RectangleEntity() {}

void InterlacedRectangleEntity::addShaders() {
  m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/stereo/overlap.vert");
  m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/stereo/interlaced.frag");
}
