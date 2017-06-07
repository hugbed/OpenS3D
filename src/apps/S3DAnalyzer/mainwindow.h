#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

#include "rendering/openglwidget.h"

// todo: use forwarding everywhere possible
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = 0);
  ~MainWindow();

  void computeAndUpdate();
  void updateConvergenceHint(float minDisparity, float maxDisparity);

 private:
  template <class Functor>
  void requestImageFilename(Functor f);

  QImage m_imageLeft;
  QImage m_imageRight;

  Ui::MainWindow* ui;
};

#endif  // MAINWINDOW_H
