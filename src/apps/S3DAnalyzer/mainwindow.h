#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

#include "rendering/openglwidget.h"
#include "rendering/openglrenderer.h"

namespace Ui {
class MainWindow;
}  // namespace Ui

struct RenderingContext {
  explicit RenderingContext(OpenGLRenderer* renderer) : openGLRenderer{renderer} {
    textureManager = openGLRenderer->createTextureManager();
    entityManager = openGLRenderer->createEntityManager(textureManager.get());
    openGLRenderer->setEntityManager(entityManager.get());
  }

  ~RenderingContext() {
    makeCurrent();
    entityManager.reset();
    textureManager.reset();
    doneCurrent();
  }

  void makeCurrent() { openGLRenderer->makeCurrent(); }

  void doneCurrent() { openGLRenderer->doneCurrent(); }

  std::unique_ptr<TextureManager> textureManager;
  std::unique_ptr<EntityManager> entityManager;
  OpenGLRenderer* openGLRenderer;
};

class OpenGLWindow;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override;

  void computeAndUpdate();
  void updateConvergenceHint(float minDisparity, float maxDisparity);

 protected:
  void mouseDoubleClickEvent(QMouseEvent* e) override;

 private:
  void updateRenderingContexts();
  EntityManager::DisplayMode getCurrentDisplayMode();

  template <class Functor>
  void requestImageFilename(Functor f);

  std::unique_ptr<RenderingContext> m_widgetRenderingContext;
  std::unique_ptr<RenderingContext> m_windowRenderingContext;
  std::vector<RenderingContext*> m_renderingContexts;
  std::unique_ptr<OpenGLWindow> m_openGLWindow;

  // cached info
  std::vector<QVector2D> m_featurePoints;
  std::vector<float> m_disparitiesPercent;

  Ui::MainWindow* ui;
};

#endif  // MAINWINDOW_H
