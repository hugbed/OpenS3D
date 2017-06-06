#include "anaglyphrectangleentity.h"

AnaglyphRectangleEntity::AnaglyphRectangleEntity()
    : RectangleEntity()
{
}

void AnaglyphRectangleEntity::addShaders()
{
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/stereo/overlap.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/stereo/anaglyph.frag");
}
