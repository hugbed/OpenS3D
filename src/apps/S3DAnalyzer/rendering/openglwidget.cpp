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
  for (auto&& texture : m_textures) {
    texture.reset();
  }

  for (auto&& entity : m_stereoEntities) {
    entity.reset();
  }

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
  auto&& texture = m_textures[0];
  texture->destroy();
  texture->create();
  texture->setData(image.mirrored(false, true));
  setTextureDefaultProperties(texture.get());

  m_overlay->clear();

  update();
}

void OpenGLWidget::setImageRight(const QImage& image) {
  auto&& texture = m_textures[1];
  texture->destroy();
  texture->create();
  texture->setData(image.mirrored(false, true));
  setTextureDefaultProperties(texture.get());

  m_overlay->clear();

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
  addTexture(":/images/left.jpg");
  addTexture(":/images/right.jpg");
}

void OpenGLWidget::addTexture(const QString& filename) {
  auto texture = std::make_unique<QOpenGLTexture>((QImage(filename).mirrored()));
  setTextureDefaultProperties(texture.get());
  m_textures.push_back(std::move(texture));
}

void OpenGLWidget::setTextureDefaultProperties(QOpenGLTexture* texture) {
  texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
  texture->setMagnificationFilter(QOpenGLTexture::Linear);
  texture->setWrapMode(QOpenGLTexture::ClampToBorder);
  texture->setBorderColor(Qt::black);
}

void OpenGLWidget::initEntities() {
  createEntity<AnaglyphRectangleEntity>(DisplayMode::Anaglyph);
  createEntity<OpacityRectangleEntity>(DisplayMode::Opacity);
  createEntity<InterlacedRectangleEntity>(DisplayMode::Interlaced);
  createEntity<StereoSideBySideEntity>(DisplayMode::SideBySide);
  createEntity<LeftRectangleEntity>(DisplayMode::Left);
  createEntity<RightRectangleEntity>(DisplayMode::Right);

  m_currentMode = DisplayMode::Anaglyph;

  if (m_textures.empty() || m_textures[0] == nullptr) {
    return;
  }

  float w = m_textures[0]->width();
  float h = m_textures[0]->height();
  auto billboardEntity = std::make_unique<BillboardIntensityEntity>(QSize(w, h));
  billboardEntity->init();
  billboardEntity->setPoints(
      {QVector2D(0.0f, 0.0f), QVector2D(w / 2.0f, h / 2.0f), QVector2D(w - 1.0f, h - 1.0f)},
      {-1.0f, 0.0f, 0.8f});
  billboardEntity->setMinIntensity(-1.1f);
  billboardEntity->setMaxIntensity(3.1f);
  m_overlay = std::move(billboardEntity);
}

void OpenGLWidget::drawEntities() {
  auto ratio = computeImageAspectRatio();

  auto&& currentEntity = m_stereoEntities[static_cast<int>(m_currentMode)];
  currentEntity->setAspectRatio(ratio);
  currentEntity->setHorizontalShift(m_horizontalShift);
  currentEntity->draw();

  // todo adapt features to current mode
  if (m_showOverlay && m_overlay != nullptr) {
    m_overlay->setImageSize(QSize(m_textures[0]->width(), m_textures[0]->height()));
    m_overlay->setAspectRatio(ratio);  // necessary?
    m_overlay->setHorizontalShift(m_horizontalShift);
    m_overlay->draw();
  }
}

float OpenGLWidget::computeImageAspectRatio() {
  if (m_textures.empty() || m_textures[0] == nullptr) {
    return 1.0f;
  }

  float vW = m_viewportSize.width(), vH = m_viewportSize.height(), iW = m_textures[0]->width(),
        iH = m_textures[0]->height();

  return (vW / vH) / (iW / iH);
}

template <class T>
void OpenGLWidget::createEntity(DisplayMode mode) {
  std::unique_ptr<StereoImageEntity> stereoImageEntity = std::make_unique<T>();
  stereoImageEntity->init();
  stereoImageEntity->setTextureLeft(m_textures[0].get());
  stereoImageEntity->setTextureRight(m_textures[1].get());
  m_stereoEntities[static_cast<int>(mode)] = std::move(stereoImageEntity);
}
