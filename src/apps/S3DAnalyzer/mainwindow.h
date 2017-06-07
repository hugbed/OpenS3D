#ifndef S3D_ANALYZER_MAINWINDOW_H
#define S3D_ANALYZER_MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

#include "rendering/openglwidget.h"

// todo: use forwarding everywhere possible
namespace Ui {
class MainWindow;
}  // namespace Ui

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override;

  void computeAndUpdate();
  void updateConvergenceHint(float minDisparity, float maxDisparity);

 private:
  template <class Functor>
  void requestImageFilename(Functor f);

  QImage m_imageLeft;
  QImage m_imageRight;

  Ui::MainWindow* ui;
};

#endif  // S3D_ANALYZER_MAINWINDOW_H
