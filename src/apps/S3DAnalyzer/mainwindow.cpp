#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "worker/depthanalyzer.h"

#include <QMouseEvent>
#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  m_imageLeft = QImage(":/images/left.jpg");
  m_imageRight = QImage(":/images/right.jpg");

  connect(ui->actionOpen_Left_Image, &QAction::triggered, [this] {
    requestImageFilename([this](const QString& filename) {
      QImage img(filename);
      ui->openGLWidget->setImageLeft(img);
      m_imageLeft = img;
    });
  });

  connect(ui->actionOpen_Right_Image, &QAction::triggered, [this] {
    requestImageFilename([this](const QString& filename) {
      QImage img(filename);
      ui->openGLWidget->setImageRight(QImage(filename));
      m_imageRight = img;
    });
  });

  connect(ui->actionAnaglyph, &QAction::triggered,
          [this] { ui->openGLWidget->displayModeChanged(OpenGLWidget::DisplayMode::Anaglyph); });
  connect(ui->actionOpacity, &QAction::triggered,
          [this] { ui->openGLWidget->displayModeChanged(OpenGLWidget::DisplayMode::Opacity); });
  connect(ui->actionInterlaced, &QAction::triggered,
          [this] { ui->openGLWidget->displayModeChanged(OpenGLWidget::DisplayMode::Interlaced); });
  connect(ui->actionSide_by_side, &QAction::triggered,
          [this] { ui->openGLWidget->displayModeChanged(OpenGLWidget::DisplayMode::SideBySide); });
  connect(ui->actionLeft, &QAction::triggered,
          [this] { ui->openGLWidget->displayModeChanged(OpenGLWidget::DisplayMode::Left); });
  connect(ui->actionRight, &QAction::triggered,
          [this] { ui->openGLWidget->displayModeChanged(OpenGLWidget::DisplayMode::Right); });
  connect(ui->actionFeatures, &QAction::triggered, [this] { ui->openGLWidget->toggleFeatures(); });
  connect(ui->actionCompute, &QAction::triggered, [this] { computeAndUpdate(); });

  connect(ui->hitWidget,
          static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
          [this](double value) {
            ui->openGLWidget->setHorizontalShift(value / 100.0f);
            ui->depthWidget->setShift(value);
          });
}

MainWindow::~MainWindow() {
  delete ui;
}

std::mt19937 gen{};

void MainWindow::computeAndUpdate() {
  DepthAnalyzer analyzer;
  analyzer.analyze(m_imageLeft, m_imageRight);

  ui->depthWidget->setLowValue(analyzer.minDisparity);
  ui->depthWidget->setHighValue(analyzer.maxDisparity);

  ui->parametersListView->setParameter("Roll", analyzer.roll);
  ui->parametersListView->setParameter("Vertical", analyzer.vertical);
  ui->parametersListView->setParameter("Pan Keystone", analyzer.panKeystone);
  ui->parametersListView->setParameter("Tilt Keystone", analyzer.tiltKeystone);
  ui->parametersListView->setParameter("Tilt Offset", analyzer.tiltOffset);
  ui->parametersListView->setParameter("Zoom", analyzer.zoom);

  ui->openGLWidget->setFeatures(analyzer.featurePoints, analyzer.disparitiesPercent);

  updateConvergenceHint(analyzer.minDisparity, analyzer.maxDisparity);
}

void MainWindow::updateConvergenceHint(float minDisparity, float maxDisparity) {
  auto range = maxDisparity - minDisparity;

  // todo: set this in mainwindow, not inside widgets
  auto expectedMin = -1.1f;
  auto expectedMax = 3.1f;
  auto expectedRange = expectedMax - expectedMin;
  auto ratio = range / expectedRange;
  ui->convergenceIndicator->updateHint(ratio);
}

template <class Functor>
void MainWindow::requestImageFilename(Functor f) {
  QString filename = QFileDialog::getOpenFileName(this, tr("Open Image"), "/home/jon/Videos",
                                                  tr("Image Files (*.png *.jpg *.bmp *.pbm)"));
  if (!filename.isEmpty()) {
    f(filename);
  }
}
