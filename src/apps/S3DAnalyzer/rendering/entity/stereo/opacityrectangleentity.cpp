#include "opacityrectangleentity.h"

OpacityRectangleEntity::OpacityRectangleEntity() : RectangleEntity() {}

void OpacityRectangleEntity::addShaders() {
  m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/stereo/overlap.vert");
  m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/stereo/opacity.frag");
}
