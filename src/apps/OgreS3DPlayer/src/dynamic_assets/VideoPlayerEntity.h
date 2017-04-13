//
// Created by jon on 13/04/17.
//

#ifndef PROJECT_VIDEOPLAYERENTITY_H
#define PROJECT_VIDEOPLAYERENTITY_H

#include "../video_texture/DynamicTexture.hpp"

#include <OgreRectangle2D.h>
#include <OgreRenderable.h>

#include <memory>

class VideoPlayerEntity : public Ogre::MovableObject {
 public:
  VideoPlayerEntity(std::string entityName, std::string materialName)
      : Ogre::MovableObject(entityName), materialName_{std::move(materialName)} {
    rectangle_ = createRectangle({-1.0f, 1.0f}, materialName_);
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

  // todo(hugbed): code repetition with VideoPlayer3DEntity
  static std::unique_ptr<Ogre::Rectangle2D> createRectangle(std::pair<float, float> boundaries,
                                                            const std::string& materialName) {
    auto rect = std::make_unique<Ogre::Rectangle2D>(true);
    rect->setCorners(boundaries.first, 1.0f, boundaries.second, -1.0f);
    rect->setMaterial(materialName);

    // Render the background before everything else
    rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);

    // Use infinite AAB to always stay visible
    Ogre::AxisAlignedBox aabInf;
    aabInf.setInfinite();
    rect->setBoundingBox(aabInf);

    return rect;
  }

  static std::string movableType_;

  std::string materialName_;
  std::unique_ptr<Ogre::Rectangle2D> rectangle_;
};

#endif  // PROJECT_VIDEOPLAYERENTITY_H
