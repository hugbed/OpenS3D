#ifndef RENDERING_OPENGLWIDGET_H
#define RENDERING_OPENGLWIDGET_H

#include <QImage>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include "entity/entitymanager.h"
#include "rendering/openglrenderer.h"
#include "texturemanager.h"

#include <gsl/span>

#include <memory>
#include <vector>

class QOpenGLShaderProgram;

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions, public OpenGLRenderer {
  Q_OBJECT

 public:
  ~OpenGLWidget() override;
  explicit OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {}

  void update() override;
  void setEntityManager(EntityManager* entityManager) override;
  std::unique_ptr<TextureManager> createTextureManager() override;
  std::unique_ptr<EntityManager> createEntityManager(TextureManager* textureManager) override;

  void makeCurrent() override { QOpenGLWidget::makeCurrent(); }
  void doneCurrent() override { QOpenGLWidget::doneCurrent(); }

  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

 signals:
  void GLInitialized();

 private:
  QSize m_viewportSize;

  EntityManager* m_entityManager;
};

#endif  // RENDERING_OPENGLWIDGET_H
