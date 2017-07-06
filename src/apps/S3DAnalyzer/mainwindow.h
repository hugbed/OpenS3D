#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "rendering/entity/entitymanager.h"

namespace Ui {
class MainWindow;
}  // namespace Ui

class OpenGLWindow;
class DepthAnalyzer;
class RenderingContext;
class VideoSynchronizer;

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
