#ifndef RENDERING_OPENGLRENDERER_H
#define RENDERING_OPENGLRENDERER_H

#include <memory>

class TextureManager;
class EntityManager;

class OpenGLRenderer {
 public:
  virtual void update() = 0;
  virtual void setEntityManager(EntityManager* entityManager) = 0;
  virtual std::unique_ptr<TextureManager> createTextureManager() = 0;
  virtual std::unique_ptr<EntityManager> createEntityManager(TextureManager* textureManager) = 0;
  virtual void makeCurrent() = 0;
  virtual void doneCurrent() = 0;
};

#endif  // RENDERING_OPENGLRENDERER_H
