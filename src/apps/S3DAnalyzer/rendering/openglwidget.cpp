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
  m_textureManager.reset();

  doneCurrent();
}

void OpenGLWidget::initializeGL() {
  // Initialize OpenGL Backend
  initializeOpenGLFunctions();

  // Set global information
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_PROGRAM_POINT_SIZE);

  initTextures();
  initEntities();
}

void OpenGLWidget::resizeGL(int w, int h) {
  // remember viewport size for drawing with correct aspect ratio
  m_viewportSize.setWidth(w);
  m_viewportSize.setHeight(h);
}

void OpenGLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT);

  drawEntities();
}

void OpenGLWidget::setImageLeft(const QImage& image) {
  m_textureManager->setImageLeft(image);
  update();
}

void OpenGLWidget::setImageRight(const QImage& image) {
  m_textureManager->setImageRight(image);
  update();
}

void OpenGLWidget::setHorizontalShift(float shift) {
  m_horizontalShift = shift;
  update();
}

void OpenGLWidget::toggleFeatures(bool display) {
  m_showOverlay = !m_showOverlay || display;
  update();
}

void OpenGLWidget::setFeatures(std::vector<QVector2D> points, std::vector<float> disparities) {
  m_overlay->setPoints(points, disparities);
  update();
}

void OpenGLWidget::displayModeChanged(OpenGLWidget::DisplayMode mode) {
  m_currentMode = mode;
  update();
}

void OpenGLWidget::initTextures() {
  m_textureManager = std::make_unique<TextureManager>();
}

void OpenGLWidget::initEntities() {
  createEntity<AnaglyphRectangleEntity>(DisplayMode::Anaglyph);
  createEntity<OpacityRectangleEntity>(DisplayMode::Opacity);
  createEntity<InterlacedRectangleEntity>(DisplayMode::Interlaced);
  createEntity<StereoSideBySideEntity>(DisplayMode::SideBySide);
  createEntity<LeftRectangleEntity>(DisplayMode::Left);
  createEntity<RightRectangleEntity>(DisplayMode::Right);

  m_currentMode = DisplayMode::Anaglyph;

  if (m_textureManager->empty()) {
    return;
  }

  auto billboardEntity = std::make_unique<BillboardIntensityEntity>(m_textureManager->getTextureSize());
  billboardEntity->init();
  billboardEntity->setPoints({}, {});
  billboardEntity->setMinIntensity(-1.1f);
  billboardEntity->setMaxIntensity(3.1f);
  m_overlay = std::move(billboardEntity);
}

void OpenGLWidget::drawEntities() {
  auto ratio = m_textureManager->computeImageAspectRatio(m_viewportSize);

  auto&& currentEntity = m_stereoEntities[static_cast<int>(m_currentMode)];
  currentEntity->setAspectRatio(ratio);
  currentEntity->setHorizontalShift(m_horizontalShift);
  currentEntity->draw();

  // todo adapt features to current mode
  if (m_showOverlay && m_overlay != nullptr) {
    m_overlay->setImageSize(m_textureManager->getTextureSize());
    m_overlay->setAspectRatio(ratio);  // necessary?
    m_overlay->setHorizontalShift(m_horizontalShift);
    m_overlay->draw();
  }
}

template <class T>
void OpenGLWidget::createEntity(DisplayMode mode) {
  std::unique_ptr<StereoImageEntity> stereoImageEntity = std::make_unique<T>();
  stereoImageEntity->init();
  stereoImageEntity->setTextureLeft(m_textureManager->getTexture(0));
  stereoImageEntity->setTextureRight(m_textureManager->getTexture(1));
  m_stereoEntities[static_cast<int>(mode)] = std::move(stereoImageEntity);
}
