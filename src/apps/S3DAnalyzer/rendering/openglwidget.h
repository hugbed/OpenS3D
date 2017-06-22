#ifndef S3D_ANALYZER_RENDERING_OPENGLWIDGET_H
#define S3D_ANALYZER_RENDERING_OPENGLWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QImage>

#include "rendering/entity/billboard/billboardintensityentity.h"
#include "entity/entitymanager.h"
#include "entity/stereo/rectangleentity.h"
#include "entity/stereo/stereoimageentity.h"
#include "texturemanager.h"

#include <memory>
#include <vector>
#include <gsl/span>

class QOpenGLShaderProgram;

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT

 public:
  ~OpenGLWidget() override;
  explicit OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {}

  void setEntityManager(EntityManager* entityManager);
  std::unique_ptr<TextureManager> createTextureManager();
  std::unique_ptr<EntityManager> createEntityManager(TextureManager* textureManager);

  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

 signals:
  void GLInitialized();

 private:
  QSize m_viewportSize;

  EntityManager* m_entityManager;
};

#endif  // S3D_ANALYZER_RENDERING_OPENGLWIDGET_H
