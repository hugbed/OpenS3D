#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "rendering/openglwindow.h"
#include "rendering/renderingcontext.h"
#include "rendering/texturemanager.h"
#include "worker/videosynchronizer.h"
#include "widgets/settingsdialog.h"
#include "utilities/cv.h"

#include <s3d/cv/disparity/disparity_analyzer_stan.h>

#include <QFileDialog>
#include <QMouseEvent>
#include <QtGui/QPainter>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_settingsDialog{std::make_unique<SettingsDialog>(this)} {
  ui->setupUi(this);

  ui->actionOpenLeftVideo->setVisible(true);
  ui->actionOpenRightVideo->setVisible(true);
  ui->actionOpenLeftImage->setVisible(false);
  ui->actionOpenRightImage->setVisible(false);
  ui->actionOpenVideo->setVisible(false);
  ui->actionOpenImage->setVisible(false);

  m_settingsDialog->setUserSettings(&m_userSettings);
  m_videoSynchronizer = std::make_unique<VideoSynchronizer>();
  m_videoSynchronizer->setStereoVideoFormat(s3d::Stereo3DFormat::Separate);

  // todo: group connects in separate functions
  // this function is getting laaarge

  m_analyzer = std::make_unique<s3d::DisparityAnalyzerSTAN>(m_analyzerSmoothingFactor);

  ui->depthWidget->setDisplayRange(m_userSettings.displayParameters.displayRangeMin,
                                   m_userSettings.displayParameters.displayRangeMax);
  ui->depthWidget->setExpectedRange(m_userSettings.displayParameters.expectedRangeMin,
                                    m_userSettings.displayParameters.expectedRangeMax);

  connect(m_settingsDialog.get(), &SettingsDialog::settingsUpdated,
          [this](UserSettings userSettings) {
            // keep image width in pixels that is not controlled by the user
            int imageWidthPixels = m_userSettings.viewerContext.imageWidthPixels;
            m_userSettings = std::move(userSettings);
            m_userSettings.viewerContext.imageWidthPixels = imageWidthPixels;

            m_currentContext->entityManager->setUserSettings(&m_userSettings);
            m_currentContext->openGLRenderer->updateScene();

            ui->depthWidget->setDisplayRange(m_userSettings.displayParameters.displayRangeMin,
                                             m_userSettings.displayParameters.displayRangeMax);
            ui->depthWidget->setExpectedRange(m_userSettings.displayParameters.expectedRangeMin,
                                              m_userSettings.displayParameters.expectedRangeMax);

            ui->convergenceIndicator->update();
          });

  connect(ui->actionInputImage, &QAction::triggered, [this] { updateInputMode(); });
  connect(ui->actionInputVideo, &QAction::triggered, [this] { updateInputMode(); });
  connect(ui->actionInputLive, &QAction::triggered, [this] { updateInputMode(); });

  connect(ui->actionFormatSeparateFiles, &QAction::triggered, [this] {
    updateInputMode();
    updateStereo3DFormat();
  });
  connect(ui->actionFormatSideBySide, &QAction::triggered, [this] {
    updateInputMode();
    updateStereo3DFormat();
  });
  connect(ui->actionFormatAboveBelow, &QAction::triggered, [this] {
    updateInputMode();
    updateStereo3DFormat();
  });
  connect(ui->actionFormatHalfResolution, &QAction::triggered, [this] { updateStereo3DFormat(); });

  connect(ui->actionSettings, &QAction::triggered, [this] { m_settingsDialog->show(); });

  connect(ui->actionOpenLeftImage, &QAction::triggered, [this] {
    requestImageFilename([this](const QString& filename) {
      if (m_videoSynchronizer != nullptr) {
        m_videoSynchronizer->stop();
        ui->videoControls->pause();
      }

      QImage img(filename);
      m_currentContext->makeCurrent();
      m_currentContext->textureManager->setImageLeft(img);
      m_currentContext->doneCurrent();
      m_currentContext->openGLRenderer->updateScene();
    });
  });

  connect(ui->actionOpenRightImage, &QAction::triggered, [this] {
    requestImageFilename([this](const QString& filename) {
      if (m_videoSynchronizer != nullptr) {
        m_videoSynchronizer->stop();
        ui->videoControls->pause();
      }

      QImage img(filename);
      m_currentContext->makeCurrent();
      m_currentContext->textureManager->setImageRight(img);
      m_currentContext->doneCurrent();
      m_currentContext->openGLRenderer->updateScene();
    });
  });

  connect(ui->actionOpenImage, &QAction::triggered, [this] {
    requestImageFilename([this](const QString& filename) {
      if (m_videoSynchronizer != nullptr) {
        m_videoSynchronizer->stop();
        ui->videoControls->pause();
      }

      // todo: DEMUX THIS IMAGE
      QImage img(filename);
      m_currentContext->makeCurrent();
      m_currentContext->textureManager->setImageLeft(img);
      m_currentContext->doneCurrent();
      m_currentContext->openGLRenderer->updateScene();
    });
  });

  connect(ui->actionOpenLeftVideo, &QAction::triggered, [this] {
    requestVideoFilename([this](const QString& filename) {
      if (m_videoSynchronizer != nullptr) {
        m_videoSynchronizer->stop();
        ui->videoControls->pause();
        m_videoSynchronizer->setLeftFilename(filename.toUtf8().toStdString());

        // if both files set, load video
        if (m_videoSynchronizer->isVideoReadyToLoad()) {
          m_videoSynchronizer->loadStereoVideo();
        }
      }
    });
  });

  connect(ui->actionOpenRightVideo, &QAction::triggered, [this] {
    requestVideoFilename([this](const QString& filename) {
      if (m_videoSynchronizer != nullptr) {
        m_videoSynchronizer->stop();
        ui->videoControls->pause();
        m_videoSynchronizer->setRightFilename(filename.toUtf8().toStdString());

        // if both files set, load video
        if (m_videoSynchronizer->isVideoReadyToLoad()) {
          m_videoSynchronizer->loadStereoVideo();
        }
      }
    });
  });

  connect(ui->actionOpenVideo, &QAction::triggered, [this] {
    requestVideoFilename([this](const QString& filename) {
      if (m_videoSynchronizer != nullptr) {
        m_videoSynchronizer->stop();
        ui->videoControls->pause();
        m_videoSynchronizer->setLeftFilename(filename.toUtf8().toStdString());

        if (m_videoSynchronizer->isVideoReadyToLoad()) {
          m_videoSynchronizer->loadStereoVideo();
        }
      }
    });
  });

  connect(ui->actionAnaglyph, &QAction::triggered, [this] {
    m_currentContext->entityManager->displayModeChanged(EntityManager::DisplayMode::Anaglyph);
    m_currentContext->openGLRenderer->updateScene();
  });

  connect(ui->actionOpacity, &QAction::triggered, [this] {
    m_currentContext->entityManager->displayModeChanged(EntityManager::DisplayMode::Opacity);
    m_currentContext->openGLRenderer->updateScene();
  });

  connect(ui->actionInterlaced, &QAction::triggered, [this] {
    m_currentContext->entityManager->displayModeChanged(EntityManager::DisplayMode::Interlaced);
    m_currentContext->openGLRenderer->updateScene();
  });

  connect(ui->actionSide_by_side, &QAction::triggered, [this] {
    m_currentContext->entityManager->displayModeChanged(EntityManager::DisplayMode::SideBySide);
    m_currentContext->openGLRenderer->updateScene();
  });

  connect(ui->actionLeft, &QAction::triggered, [this] {
    m_currentContext->entityManager->displayModeChanged(EntityManager::DisplayMode::Left);
    m_currentContext->openGLRenderer->updateScene();
  });

  connect(ui->actionRight, &QAction::triggered, [this] {
    m_currentContext->entityManager->displayModeChanged(EntityManager::DisplayMode::Right);
    m_currentContext->openGLRenderer->updateScene();
  });

  connect(ui->actionViewer, &QAction::triggered, [this] {
    m_currentContext->entityManager->displayModeChanged(EntityManager::DisplayMode::ViewerCentric);
    m_currentContext->openGLRenderer->updateScene();
  });

  connect(ui->actionFeatures, &QAction::triggered,  //
          [this] {
            m_currentContext->entityManager->setFeaturesVisibility(ui->actionFeatures->isChecked());
            m_currentContext->openGLRenderer->updateScene();
          });

  connect(ui->actionEnableComputations, &QAction::triggered,  //
          [this] { computeAndUpdate(); });

  connect(ui->hitWidget,
          static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
          [this](double value) {
            m_currentContext->entityManager->setHorizontalShift(value / 100.0f);
            m_currentContext->openGLRenderer->updateScene();
            ui->depthWidget->setShift(value);
          });

  connect(ui->openGLWidget, &OpenGLWidget::GLInitialized, [this] {
    m_widgetRenderingContext = std::make_unique<RenderingContext>(ui->openGLWidget);
    m_currentContext = m_widgetRenderingContext.get();
    m_currentContext->entityManager->setUserSettings(&m_userSettings);
    m_userSettings.viewerContext.imageWidthPixels =
        m_currentContext->textureManager->getTextureSize().width();

    connect(
        m_videoSynchronizer.get(), &VideoSynchronizer::incomingImagePair,
        [this](const QImage& imgLeft, const QImage& imgRight, std::chrono::microseconds timestamp) {
          m_currentContext->makeCurrent();
          m_currentContext->textureManager->setImages(imgLeft, imgRight);
          m_currentContext->doneCurrent();
          ui->videoControls->updateSlider(timestamp);
          m_userSettings.viewerContext.imageWidthPixels = imgLeft.width();
          m_currentContext->entityManager->setUserSettings(&m_userSettings);
          computeAndUpdate();
        });

    connect(ui->videoControls, &VideoControls::playClicked, [this] {
      if (m_videoSynchronizer != nullptr) {
        m_videoSynchronizer->resume();
        ui->videoControls->setDuration(m_videoSynchronizer->videoDuration());
      }
    });
    connect(ui->videoControls, &VideoControls::firstClicked, [this] {
      if (m_videoSynchronizer != nullptr) {
        m_videoSynchronizer->seekTo(std::chrono::microseconds{0});
      }
    });
    connect(ui->videoControls, &VideoControls::pauseClicked, [this] {
      if (m_videoSynchronizer != nullptr) {
        m_videoSynchronizer->pause();
      }
    });
    connect(ui->videoControls, &VideoControls::nextClicked, [this] {
      if (m_videoSynchronizer != nullptr) {
        m_videoSynchronizer->next();
      }
    });
  });

  connect(ui->videoControls, &VideoControls::seekingRequested,
          [this](std::chrono::microseconds timestamp) { m_videoSynchronizer->seekTo(timestamp); });
}

MainWindow::~MainWindow() {
  // delete here to make OpenGL context current
  if (m_videoSynchronizer != nullptr) {
    m_videoSynchronizer->stop();
  }
  m_widgetRenderingContext.reset();
  m_windowRenderingContext.reset();
  delete ui;
}

void MainWindow::computeAndUpdate() {
  if (ui->actionEnableComputations->isChecked()) {
    QImage imageLeft = m_currentContext->textureManager->getImageLeft();
    QImage imageRight = m_currentContext->textureManager->getImageRight();

    if (m_analyzer->analyze(QImage2Mat(imageLeft), QImage2Mat(imageRight)) &&
        (m_analyzer->results.maxDisparityPercent - m_analyzer->results.minDisparityPercent) >
            0.005) {
      ui->depthWidget->setLowValue(m_analyzer->results.minDisparityPercent);
      ui->depthWidget->setHighValue(m_analyzer->results.maxDisparityPercent);

      ui->parametersListView->setParameter("Roll", m_analyzer->results.roll);
      ui->parametersListView->setParameter("Vertical", m_analyzer->results.vertical);
      ui->parametersListView->setParameter("Pan Keystone", m_analyzer->results.panKeystone);
      ui->parametersListView->setParameter("Tilt Keystone", m_analyzer->results.tiltKeystone);
      ui->parametersListView->setParameter("Tilt Offset", m_analyzer->results.tiltOffset);
      ui->parametersListView->setParameter("Zoom", m_analyzer->results.zoom);

      m_currentContext->makeCurrent();
      m_currentContext->entityManager->setFeatures(m_analyzer->results.featurePoints,
                                                   m_analyzer->results.disparitiesPercent);
      m_currentContext->doneCurrent();
      updateConvergenceHint(m_analyzer->results.minDisparityPercent,
                            m_analyzer->results.maxDisparityPercent);
    } else {
      m_currentContext->openGLRenderer->makeCurrent();
      m_currentContext->entityManager->setFeatures({}, {});
      m_currentContext->openGLRenderer->doneCurrent();
    }
  }
  m_currentContext->openGLRenderer->updateScene();
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
  if (not filename.isEmpty()) {
    f(filename);
  }
}

template <class Functor>
void MainWindow::requestVideoFilename(Functor f) {
  QString filename = QFileDialog::getOpenFileName(this, tr("Open Video"), "/home/jon/Videos");
  if (not filename.isEmpty()) {
    f(filename);
  }
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* e) {
  if (e->button() == Qt::LeftButton) {
    if (m_openGLWindow == nullptr) {
      m_openGLWindow = std::make_unique<OpenGLWindow>();

      //      QSurfaceFormat format;
      //      format.setRenderableType(QSurfaceFormat::OpenGL);
      //      format.setProfile(QSurfaceFormat::CoreProfile);
      //      format.setVersion(3, 3);
      //      m_openGLWindow->setFormat(format);
      m_openGLWindow->resize(800, 600);

      connect(m_openGLWindow.get(), &OpenGLWindow::GLInitialized, [this] {
        // copy current context (images, features, shift) to window context
        // since everything before the double click will not have been registered
        m_windowRenderingContext = std::make_unique<RenderingContext>(m_openGLWindow.get());
        m_windowRenderingContext->persistState(m_widgetRenderingContext.get(),  //
                                               getCurrentDisplayMode(),         //
                                               m_analyzer->results.featurePoints,
                                               m_analyzer->results.disparitiesPercent,
                                               ui->actionFeatures->isChecked());
        m_windowRenderingContext->entityManager->setUserSettings(&m_userSettings);
        m_currentContext = m_windowRenderingContext.get();
      });

      connect(m_openGLWindow.get(), &OpenGLWindow::onClose, [this] {
        m_widgetRenderingContext->persistState(m_windowRenderingContext.get(),  //
                                               getCurrentDisplayMode(),         //
                                               m_analyzer->results.featurePoints,
                                               m_analyzer->results.disparitiesPercent,
                                               ui->actionFeatures->isChecked());
        m_currentContext = m_widgetRenderingContext.get();
      });
    } else {
      m_windowRenderingContext->persistState(m_widgetRenderingContext.get(),  //
                                             getCurrentDisplayMode(),         //
                                             m_analyzer->results.featurePoints,
                                             m_analyzer->results.disparitiesPercent,
                                             ui->actionFeatures->isChecked());
      m_currentContext = m_windowRenderingContext.get();
    }

    m_openGLWindow->showFullScreen();
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

void MainWindow::closeEvent(QCloseEvent* event) {
  if (m_settingsDialog != nullptr) {
    m_settingsDialog->close();
  }
}

void MainWindow::updateInputMode() {
  // hide all menus
  ui->actionOpenLeftImage->setVisible(false);
  ui->actionOpenRightImage->setVisible(false);
  ui->actionOpenImage->setVisible(false);
  ui->actionOpenLeftVideo->setVisible(false);
  ui->actionOpenRightVideo->setVisible(false);
  ui->actionOpenVideo->setVisible(false);

  // show only relevant menus
  if (not ui->actionFormatSeparateFiles->isChecked()) {
    if (ui->actionInputImage->isChecked()) {
      ui->actionOpenImage->setVisible(true);
    } else if (ui->actionInputVideo->isChecked()) {
      ui->actionOpenVideo->setVisible(true);
    }
  } else {
    if (ui->actionInputImage->isChecked()) {
      ui->actionOpenLeftImage->setVisible(true);
      ui->actionOpenRightImage->setVisible(true);
    } else if (ui->actionInputVideo->isChecked()) {
      ui->actionOpenLeftVideo->setVisible(true);
      ui->actionOpenRightVideo->setVisible(true);
    }
  }

  // no value smoothing for image
  if (ui->actionInputImage->isChecked()) {
    m_analyzer->setSmoothingFactor(1.0f);
  } else {
    m_analyzer->setSmoothingFactor(m_analyzerSmoothingFactor);
  }
}

void MainWindow::updateStereo3DFormat() {
  bool halfResolution = ui->actionFormatHalfResolution->isChecked();

  s3d::Stereo3DFormat format{};
  if (ui->actionFormatSeparateFiles->isChecked()) {
    format = s3d::Stereo3DFormat::Separate;
  } else if (ui->actionFormatSideBySide->isChecked()) {
    format = halfResolution ? s3d::Stereo3DFormat::SideBySideHalf : s3d::Stereo3DFormat::SideBySide;
  } else if (ui->actionFormatAboveBelow->isChecked()) {
    format = halfResolution ? s3d::Stereo3DFormat::AboveBelowHalf : s3d::Stereo3DFormat::AboveBelow;
  }
  if (m_videoSynchronizer != nullptr) {
    m_videoSynchronizer->setStereoVideoFormat(format);
  }
}
