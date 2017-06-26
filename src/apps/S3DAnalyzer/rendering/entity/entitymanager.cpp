#include <gsl/gsl_util>
#include "entitymanager.h"

#include "../texturemanager.h"

#include "stereo/rectangleentity.h"
#include "stereo/stereoimageentity.h"
#include "stereo/anaglyphrectangleentity.h"
#include "stereo/opacityrectangleentity.h"
#include "stereo/interlacedrectangleentity.h"
#include "stereo/stereosidebysideentity.h"
#include "stereo/leftrectangleentity.h"
#include "stereo/rightrectangleentity.h"
#include "billboard/billboardintensityentity.h"

EntityManager::EntityManager(TextureManager* textureManager) : m_textureManager{textureManager} {
  createEntity<AnaglyphRectangleEntity>(DisplayMode::Anaglyph);
  createEntity<OpacityRectangleEntity>(DisplayMode::Opacity);
  createEntity<InterlacedRectangleEntity>(DisplayMode::Interlaced);
  createEntity<StereoSideBySideEntity>(DisplayMode::SideBySide);
  createEntity<LeftRectangleEntity>(DisplayMode::Left);
  createEntity<RightRectangleEntity>(DisplayMode::Right);

  m_currentMode = DisplayMode::Anaglyph;

  auto billboardEntity =
      std::make_unique<BillboardIntensityEntity>(m_textureManager->getTextureSize());
  billboardEntity->init();
  billboardEntity->setPoints({}, {});
  billboardEntity->setMinIntensity(-1.1f);
  billboardEntity->setMaxIntensity(3.1f);
  m_overlay = std::move(billboardEntity);
}

EntityManager::~EntityManager() = default;

void EntityManager::drawEntities(QSize viewportSize) {
  // update dirty textures
  m_textureManager->update();

  // render entities using the updated textures
  auto ratio = m_textureManager->computeImageAspectRatio(viewportSize);

  auto&& currentEntity = gsl::at(m_stereoEntities, static_cast<int>(m_currentMode));
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
void EntityManager::createEntity(DisplayMode mode) {
  std::unique_ptr<StereoImageEntity> stereoImageEntity = std::make_unique<T>();
  stereoImageEntity->init();
  stereoImageEntity->setTextureLeft(m_textureManager->getTexture(0));
  stereoImageEntity->setTextureRight(m_textureManager->getTexture(1));
  gsl::at(m_stereoEntities, static_cast<int>(mode)) = std::move(stereoImageEntity);
}

void EntityManager::setFeaturesVisibility(bool display) {
  m_showOverlay = display;
}

void EntityManager::setFeatures(std::vector<QVector2D> points, std::vector<float> disparities) {
  m_overlay->setPoints(points, disparities);
}

void EntityManager::setHorizontalShift(float horizontalShift) {
  m_horizontalShift = horizontalShift;
}

void EntityManager::displayModeChanged(EntityManager::DisplayMode mode) {
  m_currentMode = mode;
}
