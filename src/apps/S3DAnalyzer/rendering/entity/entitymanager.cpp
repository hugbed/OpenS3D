#include <gsl/gsl_util>
#include "entitymanager.h"

#include "../texturemanager.h"

#include "billboard/billboardintensityworldentity.h"
#include "billboard/billboardintensityentity.h"
#include "stereo/rectangleentity.h"
#include "stereo/stereoimageentity.h"
#include "stereo/anaglyphrectangleentity.h"
#include "stereo/opacityrectangleentity.h"
#include "stereo/interlacedrectangleentity.h"
#include "stereo/stereosidebysideentity.h"
#include "stereo/leftrectangleentity.h"
#include "stereo/rightrectangleentity.h"
#include "stereo/viewercentricentity.h"
#include "utilities/usersettings.h"

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
      std::make_unique<BillboardIntensityEntity>(m_textureManager->getTextureSize());
  billboardEntity->init();
  billboardEntity->setPoints({}, {});
  billboardEntity->setMinIntensity(-1.0f);
  billboardEntity->setMaxIntensity(3.0f);
  m_billboardImage = std::move(billboardEntity);

  auto billboardWorld =
      std::make_unique<BillboardIntensityWorldEntity>(m_textureManager->getTextureSize());
  billboardWorld->init();
  billboardWorld->setPoints({}, {});
  billboardWorld->setMinIntensity(-1.0f);
  billboardWorld->setMaxIntensity(3.0f);
  m_billboardWorld = std::move(billboardWorld);

  m_currentBillboard = m_billboardImage.get();
}

EntityManager::~EntityManager() = default;

// todo: compute viewportSize from parent?
void EntityManager::drawEntities(QPaintDevice* parent, QSize viewportSize) {
  // update outdated textures
  if (m_textureManager->imagesDirty()) {
    m_textureManager->update();
  }

  // render entities using the updated textures
  auto ratio = m_textureManager->computeImageAspectRatio(viewportSize);

  auto&& currentEntity = gsl::at(m_stereoEntities, static_cast<int>(m_currentMode));
  currentEntity->setAspectRatio(ratio);
  currentEntity->setHorizontalShift(m_horizontalShift);
  currentEntity->draw(parent);

  auto& viewerContext = m_userSettings->viewerContext;
  auto& dispParams = m_userSettings->displayParameters;

  // todo: this function is ugly
  if (m_showOverlay && m_currentBillboard != nullptr) {
    if (m_currentMode == DisplayMode::ViewerCentric) {
      auto d = m_userSettings->viewerContext.viewerDistance;
      float minD = -(d + d / 10.0f);
      float maxD = -minD;
      m_currentBillboard->setDisplayRange(0.0f, m_textureManager->getTextureSize().width(),  //
                                          minD, maxD);
      m_viewerCentricEntity->setViewerContext(&m_userSettings->viewerContext);
      m_viewerCentricEntity->setDepthRangeMeters(minD, maxD);
    } else {
      m_currentBillboard->setDisplayRange(0, m_textureManager->getTextureSize().width(),  //
                                          0, m_textureManager->getTextureSize().height());
    }

    if (m_currentMode != DisplayMode::SideBySide) {
      m_currentBillboard->setAspectRatio(ratio);  // necessary?
      m_currentBillboard->setHorizontalShift(m_horizontalShift);
      m_currentBillboard->setMinIntensity(dispParams.expectedRangeMin);
      m_currentBillboard->setMaxIntensity(dispParams.expectedRangeMax);
      m_currentBillboard->draw(parent);
    }
  }
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

void EntityManager::setFeatures(std::vector<QVector2D> points, std::vector<float> disparities) {
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
  m_viewerCentricEntity->setViewerContext(&userSettings->viewerContext);
}
