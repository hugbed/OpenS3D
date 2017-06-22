#include "openglwidget.h"

#include "entity/stereo/anaglyphrectangleentity.h"
#include "entity/stereo/opacityrectangleentity.h"
#include "entity/stereo/interlacedrectangleentity.h"
#include "entity/stereo/stereosidebysideentity.h"
#include "entity/stereo/leftrectangleentity.h"
#include "entity/stereo/rightrectangleentity.h"

OpenGLWidget::~OpenGLWidget() {
  makeCurrent();

  // OpenGL related objects must be cleared
  // with their related current

  // ...

  doneCurrent();
}

std::unique_ptr<TextureManager> OpenGLWidget::createTextureManager() {
  makeCurrent();
  auto textureManager = std::make_unique<TextureManager>();
  doneCurrent();
  return textureManager;
}

void OpenGLWidget::setEntityManager(EntityManager* entityManager) {
  m_entityManager = entityManager;
}

std::unique_ptr<EntityManager> OpenGLWidget::createEntityManager(TextureManager* textureManager) {
  makeCurrent();
  auto entityManager = std::make_unique<EntityManager>(textureManager);
  doneCurrent();
  return entityManager;
}

void OpenGLWidget::initializeGL() {
  // Initialize OpenGL Backend
  initializeOpenGLFunctions();

  // Set global information
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_PROGRAM_POINT_SIZE);

  emit GLInitialized();
}

void OpenGLWidget::resizeGL(int w, int h) {
  // remember viewport size for drawing with correct aspect ratio
  m_viewportSize = QSize(w, h);
}

void OpenGLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT);

  if (m_entityManager != nullptr) {
    m_entityManager->drawEntities(m_viewportSize);
  }
}
