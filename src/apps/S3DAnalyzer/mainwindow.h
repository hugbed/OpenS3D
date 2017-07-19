#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "utilities/usersettings.h"
#include "rendering/entity/entitymanager.h"

namespace Ui {
class MainWindow;
}  // namespace Ui

class OpenGLWindow;
class DepthAnalyzer;
class RenderingContext;
class VideoSynchronizer;

namespace s3d {
class DisparityAnalyzerSTAN;
}

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
  std::unique_ptr<s3d::DisparityAnalyzerSTAN> m_analyzer;

  UserSettings m_userSettings{};

  Ui::MainWindow* ui;
};

#endif  // MAINWINDOW_H
