#include "renderingcontext.h"

#include "rendering/openglrenderer.h"
#include "rendering/texturemanager.h"

RenderingContext::RenderingContext(OpenGLRenderer* renderer) : openGLRenderer{renderer} {
  textureManager = openGLRenderer->createTextureManager();
  entityManager = openGLRenderer->createEntityManager(textureManager.get());
  openGLRenderer->setEntityManager(entityManager.get());
}

RenderingContext::~RenderingContext() {
  makeCurrent();
  entityManager.reset();
  textureManager.reset();
  doneCurrent();
}

void RenderingContext::persistState(RenderingContext* other,
                                    EntityManager::DisplayMode displayMode,
                                    const std::vector<Eigen::Vector2f>& featurePoints,
                                    const std::vector<float>& disparitiesPercent,
                                    bool featureDisplayed) {
  if (other == nullptr) {
    return;
  }
  entityManager->displayModeChanged(displayMode);
  makeCurrent();
  textureManager->setImageLeft(other->textureManager->getImageLeft());
  textureManager->setImageRight(other->textureManager->getImageRight());
  entityManager->setFeatures(featurePoints, disparitiesPercent);
  entityManager->setFeaturesVisibility(featureDisplayed);
  entityManager->setHorizontalShift(other->entityManager->getHorizontalShift());
  doneCurrent();
}

void RenderingContext::makeCurrent() {
  openGLRenderer->makeCurrent();
}

void RenderingContext::doneCurrent() {
  openGLRenderer->doneCurrent();
}
