#ifndef RENDERING_OPENGLWIDGET_H
#define RENDERING_OPENGLWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include "rendering/openglrenderer.h"

#include <memory>

class EntityManager;
class QOpenGLShaderProgram;
class TextureManager;

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions, public OpenGLRenderer {
  Q_OBJECT

 public:
  explicit OpenGLWidget(QWidget* parent);
  ~OpenGLWidget() override;

  void updateScene() override;
  void setEntityManager(EntityManager* entityManager) override;
  std::unique_ptr<TextureManager> createTextureManager() override;
  std::unique_ptr<EntityManager> createEntityManager(TextureManager* textureManager) override;

  void makeCurrent() override { QOpenGLWidget::makeCurrent(); }
  void doneCurrent() override { QOpenGLWidget::doneCurrent(); }

  void initializeGL() override;
  void paintGL() override;

 signals:
  void GLInitialized();

 private:
  EntityManager* m_entityManager{};
};

#endif  // RENDERING_OPENGLWIDGET_H
