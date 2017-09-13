#ifndef RENDERING_OPENGLWINDOW_H
#define RENDERING_OPENGLWINDOW_H

#include "rendering/openglrenderer.h"
#include <QOpenGLFunctions>
#include <QOpenGLWindow>

#include <memory>

class EntityManager;
class TextureManager;
class QHideEvent;

class OpenGLWindow : public QOpenGLWindow, protected QOpenGLFunctions, public OpenGLRenderer {
  Q_OBJECT

  // OpenGL Events
 public:
  void initializeGL() override;
  void paintGL() override;

  // OpenGLRenderer
  void updateScene() override;
  std::unique_ptr<TextureManager> createTextureManager() override;
  std::unique_ptr<EntityManager> createEntityManager(TextureManager* textureManager) override;
  void setEntityManager(EntityManager* entityManager) override;

  void makeCurrent() override { QOpenGLWindow::makeCurrent(); }
  void doneCurrent() override { QOpenGLWindow::doneCurrent(); }

 protected:
  void mouseDoubleClickEvent(QMouseEvent* e) override;
  void hideEvent(QHideEvent* /*event*/) override;

 protected slots:
  void teardownGL();

 signals:
  void GLInitialized();
  void onClose();

 private:
  EntityManager* m_entityManager;

  // Private Helpers
  void printVersionInformation();
};

#endif  // RENDERING_OPENGLWINDOW_H
