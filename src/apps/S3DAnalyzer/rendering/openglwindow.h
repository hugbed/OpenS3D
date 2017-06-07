#ifndef S3D_ANALYZER_RENDERING_OPENGLWINDOW_H
#define S3D_ANALYZER_RENDERING_OPENGLWINDOW_H

#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>

class QOpenGLShaderProgram;

class OpenGLWindow : public QOpenGLWindow, protected QOpenGLFunctions {
  Q_OBJECT

  // OpenGL Events
 public:
  void initializeGL() override;
  void resizeGL(int width, int height) override;
  void paintGL() override;
 protected slots:
  void teardownGL();

 private:
  void initColorTriangle();
  void initTexturedTriangle();
  void drawColorTriangle();
  void drawTexturedTriangle();

  // OpenGL State Information
  QOpenGLBuffer m_vertex;
  QOpenGLVertexArrayObject m_object;
  QOpenGLTexture* m_texture;
  QOpenGLShaderProgram* m_program;

  // Private Helpers
  void printVersionInformation();
};

#endif  // S3D_ANALYZER_RENDERING_OPENGLWINDOW_H
