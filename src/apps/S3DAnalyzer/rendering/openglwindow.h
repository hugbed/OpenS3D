#ifndef RENDERING_OPENGLWINDOW_H
#define RENDERING_OPENGLWINDOW_H

#include "rendering/openglrenderer.h"

#include <QOpenGLWindow>
#include <QOpenGLFunctions>

#include <memory>

class EntityManager;
class TextureManager;

class OpenGLWindow : public QOpenGLWindow, protected QOpenGLFunctions, public OpenGLRenderer {
  Q_OBJECT

  // OpenGL Events
 public:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

  // OpenGLRenderer
  void update() override;
  std::unique_ptr<TextureManager> createTextureManager() override;
  std::unique_ptr<EntityManager> createEntityManager(TextureManager* textureManager) override;
  void setEntityManager(EntityManager* entityManager) override;

  void makeCurrent() override { QOpenGLWindow::makeCurrent(); }
  void doneCurrent() override { QOpenGLWindow::doneCurrent(); }

 protected:
  void mouseDoubleClickEvent(QMouseEvent* e) override;

 protected slots:
  void teardownGL();

 signals:
  void GLInitialized();

 private:
  QSize m_viewportSize;
  EntityManager* m_entityManager;

  // Private Helpers
  void printVersionInformation();
};

#endif  // RENDERING_OPENGLWINDOW_H
