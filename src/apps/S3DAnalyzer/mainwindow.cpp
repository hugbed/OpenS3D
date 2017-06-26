#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rendering/openglwindow.h"

#include "worker/depthanalyzer.h"

#include <QMouseEvent>
#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  connect(ui->actionOpen_Left_Image, &QAction::triggered, [this] {
    requestImageFilename([this](const QString& filename) {
      QImage img(filename);
      for (auto* context : m_renderingContexts) {
        context->makeCurrent();
        context->textureManager->setImageLeft(img);
        context->doneCurrent();
      }
      updateRenderingContexts();
    });
  });

  connect(ui->actionOpen_Right_Image, &QAction::triggered, [this] {
    requestImageFilename([this](const QString& filename) {
      QImage img(filename);
      for (auto* context : m_renderingContexts) {
        context->makeCurrent();
        context->textureManager->setImageRight(img);
        context->doneCurrent();
      }
      updateRenderingContexts();
    });
  });

  connect(ui->actionAnaglyph, &QAction::triggered, [this] {
    for (auto* context : m_renderingContexts) {
      context->entityManager->displayModeChanged(EntityManager::DisplayMode::Anaglyph);
    }
    updateRenderingContexts();
  });

  connect(ui->actionOpacity, &QAction::triggered, [this] {
    for (auto* context : m_renderingContexts) {
      context->entityManager->displayModeChanged(EntityManager::DisplayMode::Opacity);
    }
    updateRenderingContexts();
  });

  connect(ui->actionInterlaced, &QAction::triggered, [this] {
    for (auto* context : m_renderingContexts) {
      context->entityManager->displayModeChanged(EntityManager::DisplayMode::Interlaced);
    }
    updateRenderingContexts();
  });

  connect(ui->actionSide_by_side, &QAction::triggered, [this] {
    for (auto* context : m_renderingContexts) {
      context->entityManager->displayModeChanged(EntityManager::DisplayMode::SideBySide);
    }
    updateRenderingContexts();
  });

  connect(ui->actionLeft, &QAction::triggered, [this] {
    for (auto* context : m_renderingContexts) {
      context->entityManager->displayModeChanged(EntityManager::DisplayMode::Left);
    }
    updateRenderingContexts();
  });

  connect(ui->actionRight, &QAction::triggered, [this] {
    for (auto* context : m_renderingContexts) {
      context->entityManager->displayModeChanged(EntityManager::DisplayMode::Right);
    }
    updateRenderingContexts();
  });

  connect(ui->actionFeatures, &QAction::triggered,  //
          [this] {
            for (auto* context : m_renderingContexts) {
              context->entityManager->setFeaturesVisibility(ui->actionFeatures->isChecked());
            }
            updateRenderingContexts();
          });

  connect(ui->actionCompute, &QAction::triggered,  //
          [this] { computeAndUpdate(); });

  connect(ui->hitWidget,
          static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
          [this](double value) {
            for (auto* context : m_renderingContexts) {
              context->entityManager->setHorizontalShift(value / 100.0f);
            }
            updateRenderingContexts();
            ui->depthWidget->setShift(value);
          });

  connect(ui->openGLWidget, &OpenGLWidget::GLInitialized, [this] {
    m_widgetRenderingContext = std::make_unique<RenderingContext>(ui->openGLWidget);
    m_renderingContexts.push_back(m_widgetRenderingContext.get());
  });
}

MainWindow::~MainWindow() {
  // delete here to make OpenGL context current
  m_widgetRenderingContext.reset();
  m_windowRenderingContext.reset();
  delete ui;
}

void MainWindow::computeAndUpdate() {
  const QImage& imageLeft = m_widgetRenderingContext->textureManager->getImageLeft();
  const QImage& imageRight = m_widgetRenderingContext->textureManager->getImageRight();

  DepthAnalyzer analyzer;
  analyzer.analyze(imageLeft, imageRight);

  ui->depthWidget->setLowValue(analyzer.minDisparity);
  ui->depthWidget->setHighValue(analyzer.maxDisparity);

  ui->parametersListView->setParameter("Roll", analyzer.roll);
  ui->parametersListView->setParameter("Vertical", analyzer.vertical);
  ui->parametersListView->setParameter("Pan Keystone", analyzer.panKeystone);
  ui->parametersListView->setParameter("Tilt Keystone", analyzer.tiltKeystone);
  ui->parametersListView->setParameter("Tilt Offset", analyzer.tiltOffset);
  ui->parametersListView->setParameter("Zoom", analyzer.zoom);

  m_featurePoints = analyzer.featurePoints;
  m_disparitiesPercent = analyzer.disparitiesPercent;

  for (auto* context : m_renderingContexts) {
    context->makeCurrent();
    context->entityManager->setFeatures(m_featurePoints, m_disparitiesPercent);
    context->doneCurrent();
  }
  updateRenderingContexts();
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

void MainWindow::mouseDoubleClickEvent(QMouseEvent* e) {
  if (e->button() == Qt::LeftButton) {
    if (m_openGLWindow == nullptr) {
      m_openGLWindow = std::make_unique<OpenGLWindow>();

      QSurfaceFormat format;
      format.setRenderableType(QSurfaceFormat::OpenGL);
      format.setProfile(QSurfaceFormat::CoreProfile);
      format.setVersion(3, 3);
      m_openGLWindow->setFormat(format);
      m_openGLWindow->resize(800, 600);

      connect(m_openGLWindow.get(), &OpenGLWindow::GLInitialized, [this] {
        // copy current context (images, features, shift) to window context
        // since everything before the double click will not have been registered
        m_windowRenderingContext = std::make_unique<RenderingContext>(m_openGLWindow.get());
        m_windowRenderingContext->entityManager->displayModeChanged(getCurrentDisplayMode());
        m_windowRenderingContext->makeCurrent();
        m_windowRenderingContext->textureManager->setImageLeft(
            m_widgetRenderingContext->textureManager->getImageLeft());
        m_windowRenderingContext->textureManager->setImageRight(
            m_widgetRenderingContext->textureManager->getImageRight());
        m_windowRenderingContext->entityManager->setFeatures(m_featurePoints, m_disparitiesPercent);
        m_windowRenderingContext->entityManager->setFeaturesVisibility(true);
        m_windowRenderingContext->doneCurrent();
        m_renderingContexts.push_back(m_windowRenderingContext.get());
      });
    }

    m_openGLWindow->show();
  }
}

void MainWindow::updateRenderingContexts() {
  for (auto* context : m_renderingContexts) {
    context->openGLRenderer->update();
  }
}

EntityManager::DisplayMode MainWindow::getCurrentDisplayMode() {
  if (ui->actionAnaglyph->isChecked()) {
    return EntityManager::DisplayMode::Anaglyph;
  }
  if (ui->actionOpacity->isChecked()) {
    return EntityManager::DisplayMode::Opacity;
  }
  if (ui->actionInterlaced->isChecked()) {
    return EntityManager::DisplayMode::Interlaced;
  }
  if (ui->actionSide_by_side->isChecked()) {
    return EntityManager::DisplayMode::SideBySide;
  }
  if (ui->actionLeft->isChecked()) {
    return EntityManager::DisplayMode::Left;
  }
  if (ui->actionRight->isChecked()) {
    return EntityManager::DisplayMode::Right;
  }

  return EntityManager::DisplayMode::Anaglyph;
}
