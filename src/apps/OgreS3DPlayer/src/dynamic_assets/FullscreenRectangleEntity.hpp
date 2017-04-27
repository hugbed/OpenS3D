#ifndef DYNAMIC_ASSETS_FULLSCREENRECTANGLEENTITY_H
#define DYNAMIC_ASSETS_FULLSCREENRECTANGLEENTITY_H

#include "../video_texture/DynamicTexture.hpp"
#include "RectangleFactory.hpp"

#include <OgreRectangle2D.h>
#include <OgreRenderable.h>

#include <memory>

class FullscreenRectangleEntity : public Ogre::MovableObject {
 public:
  explicit FullscreenRectangleEntity(std::string entityName) : Ogre::MovableObject(entityName) {
    rectangle_ = RectangleFactory::createRectangle({-1.0f, 1.0f});
  }

  FullscreenRectangleEntity(std::string entityName, std::string materialName)
      : Ogre::MovableObject(entityName), materialName_{std::move(materialName)} {
    rectangle_ = RectangleFactory::createRectangle({-1.0f, 1.0f}, materialName_);
  }

  const void setMaterialName(std::string name) {
    materialName_ = std::move(name);
    rectangle_->setMaterial(materialName_);
  }

  /** Returns the type name of this object. */
  const Ogre::String& getMovableType(void) const override { return movableType_; }

  const Ogre::AxisAlignedBox& getBoundingBox() const override {
    return rectangle_->getBoundingBox();  // infinite
  }
  float getBoundingRadius() const override {
    return rectangle_->getBoundingRadius();  // undefined
  }

  void _updateRenderQueue(Ogre::RenderQueue* queue) override {
    queue->addRenderable(rectangle_.get());
  }

  void visitRenderables(Ogre::Renderable::Visitor* visitor, bool debugRenderables) override {
    // todo(hugbed): not sure about that
    visitor->visit(rectangle_.get(), 0, debugRenderables);
  }

  static std::string movableType_;

  std::string materialName_;
  std::unique_ptr<Ogre::Rectangle2D> rectangle_;
};

#endif  // DYNAMIC_ASSETS_FULLSCREENRECTANGLEENTITY_H
