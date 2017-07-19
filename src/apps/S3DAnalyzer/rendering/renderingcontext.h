#ifndef RENDERING_RENDERINGCONTEXT_H
#define RENDERING_RENDERINGCONTEXT_H

#include "rendering/entity/entitymanager.h"

#include <Eigen/Dense>

class OpenGLRenderer;
class TextureManager;

// Structure to hold renderer, texture manager and entity manager
// to support both OpenGLWindow and OpenGLWidget. OpenGL objects
// must be created within the necessary context (window or widget).
// since some objects can't be shared, everything is create twice
// if both window and widget needs to be supported at the same time
struct RenderingContext {
  explicit RenderingContext(OpenGLRenderer* renderer);

  ~RenderingContext();

  void persistState(RenderingContext* other,
                    EntityManager::DisplayMode displayMode,
                    const std::vector<Eigen::Vector2f>& featurePoints,
                    const std::vector<float>& disparitiesPercent,
                    bool featureDisplayed);

  void makeCurrent();
  void doneCurrent();

  std::unique_ptr<TextureManager> textureManager;
  std::unique_ptr<EntityManager> entityManager;
  OpenGLRenderer* openGLRenderer;
};

#endif  // RENDERING_RENDERINGCONTEXT_H
