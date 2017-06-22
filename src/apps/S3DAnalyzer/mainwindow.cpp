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
      m_textureManager->setImageLeft(img);
      m_imageLeft = img;
      ui->openGLWidget->update();
    });
  });

  connect(ui->actionOpen_Right_Image, &QAction::triggered, [this] {
    requestImageFilename([this](const QString& filename) {
      QImage img(filename);
      m_textureManager->setImageRight(img);
      m_imageRight = img;
      ui->openGLWidget->update();
    });
  });

  connect(ui->actionAnaglyph, &QAction::triggered, [this] {
    m_entityManager->displayModeChanged(EntityManager::DisplayMode::Anaglyph);
    ui->openGLWidget->update();
  });

  connect(ui->actionOpacity, &QAction::triggered, [this] {
    m_entityManager->displayModeChanged(EntityManager::DisplayMode::Opacity);
    ui->openGLWidget->update();
  });

  connect(ui->actionInterlaced, &QAction::triggered, [this] {
    m_entityManager->displayModeChanged(EntityManager::DisplayMode::Interlaced);
    ui->openGLWidget->update();
  });

  connect(ui->actionSide_by_side, &QAction::triggered, [this] {
    m_entityManager->displayModeChanged(EntityManager::DisplayMode::SideBySide);
    ui->openGLWidget->update();
  });

  connect(ui->actionLeft, &QAction::triggered, [this] {
    m_entityManager->displayModeChanged(EntityManager::DisplayMode::Left);
    ui->openGLWidget->update();
  });

  connect(ui->actionRight, &QAction::triggered, [this] {
    m_entityManager->displayModeChanged(EntityManager::DisplayMode::Right);
    ui->openGLWidget->update();
  });

  connect(ui->actionFeatures, &QAction::triggered,  //
          [this] {
            m_entityManager->toggleFeatures();
            ui->openGLWidget->update();
          });
  connect(ui->actionCompute, &QAction::triggered,  //
          [this] { computeAndUpdate(); });

  connect(ui->hitWidget,
          static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
          [this](double value) {
            m_entityManager->setHorizontalShift(value / 100.0f);
            ui->depthWidget->setShift(value);
            ui->openGLWidget->update();
          });

  connect(ui->openGLWidget, &OpenGLWidget::GLInitialized, [this] {
    m_textureManager = ui->openGLWidget->createTextureManager();
    m_entityManager = ui->openGLWidget->createEntityManager(m_textureManager.get());
    ui->openGLWidget->setEntityManager(m_entityManager.get());
  });
}

MainWindow::~MainWindow() {
  delete ui;
}

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

  m_entityManager->setFeatures(analyzer.featurePoints, analyzer.disparitiesPercent);
  ui->openGLWidget->update();

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
