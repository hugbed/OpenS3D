#include "openglwindow.h"

#include <QDebug>
#include <QHideEvent>
#include <QMouseEvent>

#include "entity/entitymanager.h"
#include "texturemanager.h"

#include "vertex.h"

/*******************************************************************************
 * OpenGL Events
 ******************************************************************************/

void OpenGLWindow::updateScene() {
  QOpenGLWindow::update();
}

std::unique_ptr<TextureManager> OpenGLWindow::createTextureManager() {
  makeCurrent();
  auto textureManager = std::make_unique<TextureManager>();
  doneCurrent();
  return textureManager;
}

std::unique_ptr<EntityManager> OpenGLWindow::createEntityManager(TextureManager* textureManager) {
  makeCurrent();
  auto entityManager = std::make_unique<EntityManager>(textureManager);
  doneCurrent();
  return entityManager;
}

void OpenGLWindow::setEntityManager(EntityManager* entityManager) {
  m_entityManager = entityManager;
}

void OpenGLWindow::initializeGL() {
  // Initialize OpenGL Backend
  initializeOpenGLFunctions();

  // Set global information
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_PROGRAM_POINT_SIZE);

  emit GLInitialized();
}

void OpenGLWindow::resizeGL(int w, int h) {
  // remember viewport size for drawing with correct aspect ratio
  m_viewportSize = QSize(w, h);
}

void OpenGLWindow::paintGL() {
  // Clear
  glClear(GL_COLOR_BUFFER_BIT);

  if (m_entityManager != nullptr) {
    m_entityManager->drawEntities(m_viewportSize);
  }
}

void OpenGLWindow::teardownGL() {
  // Actually destroy our OpenGL information
  // do something here?
}

/*******************************************************************************
 * Private Helpers
 ******************************************************************************/

void OpenGLWindow::printVersionInformation() {
  QString glType;
  QString glVersion;
  QString glProfile;

  // Get Version Information
  glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
  glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

// Get Profile Information
#define CASE(c)           \
  case QSurfaceFormat::c: \
    glProfile = #c;       \
    break
  switch (format().profile()) {
    CASE(NoProfile);
    CASE(CoreProfile);
    CASE(CompatibilityProfile);
  }
#undef CASE

  // qPrintable() will print our QString w/o quotes around it.
  qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";
}

void OpenGLWindow::mouseDoubleClickEvent(QMouseEvent* e) {
  if (e->button() == Qt::LeftButton) {
    close();
  }
}

void OpenGLWindow::hideEvent(QHideEvent* event) {
  emit onClose();
}
