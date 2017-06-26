#include "mainwindow.h"
#include "rendering/openglwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>

#include <cassert>

#include "worker/videosynchronizer.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  // Set OpenGL Version information
  // Note: This format must be set before show() is called.
  QSurfaceFormat format;
  format.setRenderableType(QSurfaceFormat::OpenGL);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setVersion(3, 3);

  QSurfaceFormat::setDefaultFormat(format);

  // set style
  QFile f(":/qdarkstyle/style.qss");
  assert(f.exists());
  f.open(QFile::ReadOnly | QFile::Text);
  QTextStream ts(&f);
  app.setStyleSheet(ts.readAll());

  // Set the window up
  //  OpenGLWindow window;
  //  window.setFormat(format);
  //  window.resize(QSize(800, 600));
  //  window.show();

  VideoSynchronizer s;

  // Set up window
  MainWindow w;
  w.show();

  return app.exec();
}
