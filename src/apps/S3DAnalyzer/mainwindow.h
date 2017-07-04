#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

#include "rendering/openglwidget.h"
#include "rendering/openglrenderer.h"

#include "worker/videosynchronizer.h"

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

  void persistState(RenderingContext* other,
                    EntityManager::DisplayMode displayMode,
                    const std::vector<QVector2D>& featurePoints,
                    const std::vector<float>& disparitiesPercent,
                    bool featureDisplayed) {
    if (other == nullptr) {
      return;
    }
    entityManager->displayModeChanged(displayMode);
    makeCurrent();
    textureManager->setImageLeft(other->textureManager->getImageLeft());
    textureManager->setImageRight(other->textureManager->getImageRight());
    entityManager->setFeatures(featurePoints, disparitiesPercent);
    entityManager->setFeaturesVisibility(featureDisplayed);
    entityManager->setHorizontalShift(other->entityManager->getHorizontalShift());
    doneCurrent();
  }

  void makeCurrent() { openGLRenderer->makeCurrent(); }

  void doneCurrent() { openGLRenderer->doneCurrent(); }

  std::unique_ptr<TextureManager> textureManager;
  std::unique_ptr<EntityManager> entityManager;
  OpenGLRenderer* openGLRenderer;
};

class OpenGLWindow;
class DepthAnalyzer;

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
  EntityManager::DisplayMode getCurrentDisplayMode();

  template <class Functor>
  void requestImageFilename(Functor f);

  // does this belong here
  std::unique_ptr<VideoSynchronizer> m_videoSynchronizer;

  std::unique_ptr<RenderingContext> m_widgetRenderingContext;
  std::unique_ptr<RenderingContext> m_windowRenderingContext;
  RenderingContext* m_currentContext;
  std::unique_ptr<OpenGLWindow> m_openGLWindow;

  // cached info
  std::unique_ptr<DepthAnalyzer> m_analyzer;

  Ui::MainWindow* ui;
};

#endif  // MAINWINDOW_H
