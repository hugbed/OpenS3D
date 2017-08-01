#include "entitymanager.h"

#include "../texturemanager.h"

#include "rendering/entity/billboard/billboarddepthentity.h"
#include "stereo/rectangleentity.h"
#include "stereo/anaglyphrectangleentity.h"
#include "stereo/opacityrectangleentity.h"
#include "stereo/interlacedrectangleentity.h"
#include "stereo/stereosidebysideentity.h"
#include "stereo/leftrectangleentity.h"
#include "stereo/rightrectangleentity.h"
#include "stereo/viewercentricentity.h"
#include "utilities/usersettings.h"

#include <gsl/gsl_util>

EntityManager::EntityManager(TextureManager* textureManager) : m_textureManager{textureManager} {
  createEntity<AnaglyphRectangleEntity>(DisplayMode::Anaglyph);
  createEntity<OpacityRectangleEntity>(DisplayMode::Opacity);
  createEntity<InterlacedRectangleEntity>(DisplayMode::Interlaced);
  createEntity<StereoSideBySideEntity>(DisplayMode::SideBySide);
  createEntity<LeftRectangleEntity>(DisplayMode::Left);
  createEntity<RightRectangleEntity>(DisplayMode::Right);

  // Viewer centric (keep reference for viewer context)
  auto viewerCentricEntity = std::make_unique<ViewerCentricEntity>();
  m_viewerCentricEntity = viewerCentricEntity.get();
  createEntity<ViewerCentricEntity>(DisplayMode::ViewerCentric, std::move(viewerCentricEntity));

  m_currentMode = DisplayMode::Anaglyph;

  auto billboardEntity =
      std::make_unique<BillboardDisparityEntity>(m_textureManager->getTextureSize());
  billboardEntity->init();
  billboardEntity->setPoints({}, {});
  billboardEntity->setMinIntensity(-1.0f);
  billboardEntity->setMaxIntensity(3.0f);
  m_billboardImage = std::move(billboardEntity);

  auto billboardWorld = std::make_unique<BillboardDepthEntity>(m_textureManager->getTextureSize());
  billboardWorld->init();
  billboardWorld->setPoints({}, {});
  billboardWorld->setMinIntensity(-1.0f);
  billboardWorld->setMaxIntensity(3.0f);
  m_billboardWorld = std::move(billboardWorld);

  m_currentBillboard = m_billboardImage.get();
}

EntityManager::~EntityManager() = default;

void EntityManager::drawEntities(QPaintDevice* paintDevice) {
  // update outdated textures
  if (m_textureManager->imagesDirty()) {
    m_textureManager->update();
  }
  adjustDepthRanges(paintDevice->width(), paintDevice->height());
  auto deviceSize = QSize(paintDevice->width(), paintDevice->height());
  auto ratio = m_textureManager->computeImageAspectRatio(deviceSize);
  drawCurrentEntity(paintDevice, ratio);
  drawCurrentBillboard(paintDevice, ratio);
}

template <class T>
void EntityManager::createEntity(DisplayMode mode) {
  std::unique_ptr<StereoImageEntity> stereoImageEntity = std::make_unique<T>();
  createEntity(mode, std::move(stereoImageEntity));
}

template <class T>
void EntityManager::createEntity(EntityManager::DisplayMode mode, std::unique_ptr<T> t) {
  t->init();
  t->setTextureLeft(m_textureManager->getTexture(0));
  t->setTextureRight(m_textureManager->getTexture(1));
  gsl::at(m_stereoEntities, static_cast<int>(mode)) = std::move(t);
}

void EntityManager::setFeaturesVisibility(bool display) {
  m_showOverlay = display;
}

void EntityManager::setFeatures(std::vector<Eigen::Vector2f> points,
                                std::vector<float> disparities) {
  m_billboardImage->setPoints(points, disparities);
  m_billboardWorld->setPoints(points, disparities);
}

void EntityManager::setHorizontalShift(float horizontalShift) {
  m_horizontalShift = horizontalShift;
}

float EntityManager::getHorizontalShift() const {
  return m_horizontalShift;
}

void EntityManager::displayModeChanged(EntityManager::DisplayMode mode) {
  m_currentMode = mode;
  m_currentBillboard =
      (mode == DisplayMode::ViewerCentric) ? m_billboardWorld.get() : m_billboardImage.get();
}

void EntityManager::setUserSettings(UserSettings* userSettings) {
  m_userSettings = userSettings;
  m_billboardWorld->setViewerContext(&userSettings->viewerContext);
  m_billboardWorld->setDisplayZoom(userSettings->viewerDisplayZoom);
  m_viewerCentricEntity->setViewerContext(&userSettings->viewerContext);
  m_viewerCentricEntity->setDisplayZoom(userSettings->viewerDisplayZoom);
}

EntityManager::DisplayMode EntityManager::getDisplayMode() const {
  return m_currentMode;
}

void EntityManager::adjustDepthRanges(float deviceWidth, float deviceHeight) {
  // adjust ranges
  auto& viewerContext = m_userSettings->viewerContext;

  float minY{0.0f};
  float maxY = m_textureManager->getTextureSize().height();

  if (m_currentMode == DisplayMode::ViewerCentric && m_viewerCentricEntity != nullptr) {
    auto d = viewerContext.viewerDistance;
    minY = -(d + d / 10.0f);
    maxY = -minY;
    m_viewerCentricEntity->setDepthRangeMeters(minY, maxY);
    m_viewerCentricEntity->setViewerContext(&viewerContext);

    float maxX = deviceWidth / 2.0f / m_viewerCentricEntity->getPixelToWorldRatio(deviceHeight);
    float minX = -maxX;
    m_currentBillboard->setDisplayRange(minX, maxX, minY, maxY);
  } else if (m_currentBillboard != nullptr) {
    m_currentBillboard->setDisplayRange(0, m_textureManager->getTextureSize().width(), minY, maxY);
  }
}

void EntityManager::drawCurrentEntity(QPaintDevice* paintDevice, float aspectRatio) {
  auto&& currentEntity = gsl::at(m_stereoEntities, static_cast<int>(m_currentMode));
  currentEntity->setAspectRatio(aspectRatio);
  currentEntity->setHorizontalShift(m_horizontalShift);
  currentEntity->draw(paintDevice);
}

void EntityManager::drawCurrentBillboard(QPaintDevice* paintDevice, float aspectRatio) {
  // draw billboard
  // todo: this function is ugly
  if (m_showOverlay && m_currentBillboard != nullptr) {
    if (m_currentMode != DisplayMode::SideBySide) {
      m_currentBillboard->setAspectRatio(aspectRatio);  // necessary?
      m_currentBillboard->setHorizontalShift(m_horizontalShift);
      m_currentBillboard->setMinIntensity(m_userSettings->displayParameters.expectedRangeMin);
      m_currentBillboard->setMaxIntensity(m_userSettings->displayParameters.expectedRangeMax);
      m_currentBillboard->draw(paintDevice);
    }
  }
}
