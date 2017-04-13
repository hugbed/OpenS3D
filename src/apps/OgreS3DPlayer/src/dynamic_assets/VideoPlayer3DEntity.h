//
// Created by jon on 12/04/17.
//

#ifndef DYNAMIC_ASSETS_VIDEOPLAYERENTITY_H
#define DYNAMIC_ASSETS_VIDEOPLAYERENTITY_H

#include "../video_texture/DynamicTexture.hpp"

#include <OgreRectangle2D.h>
#include <OgreRenderable.h>

#include <memory>

// todo(hugbed): do VideoPlayerEntity (for rectangle and plane)

class VideoPlayer3DEntity : public Ogre::MovableObject {
 public:
  VideoPlayer3DEntity(std::string entityName,
                      std::string materialNameLeft,
                      std::string materialNameRight)
      : Ogre::MovableObject(entityName),
        materialNames_{std::move(materialNameLeft), std::move(materialNameRight)} {
    rectangles_.first = createRectangle({-1.0f, 0.0f}, materialNames_.first);
    rectangles_.second = createRectangle({0.0f, 1.0f}, materialNames_.second);
  }

  /** Returns the type name of this object. */
  const Ogre::String& getMovableType(void) const override { return movableType_; }

  const Ogre::AxisAlignedBox& getBoundingBox() const override {
    return rectangles_.second->getBoundingBox();  // infinite
  }
  float getBoundingRadius() const override {
    return rectangles_.second->getBoundingRadius();  // undefined
  }

  void _updateRenderQueue(Ogre::RenderQueue* queue) override {
    queue->addRenderable(rectangles_.first.get());
    queue->addRenderable(rectangles_.second.get());
  }

  void visitRenderables(Ogre::Renderable::Visitor* visitor, bool debugRenderables) override {
    // todo(hugbed): not sure about that
    visitor->visit(rectangles_.first.get(), 0, debugRenderables);
    visitor->visit(rectangles_.second.get(), 0, debugRenderables);
  }

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

  std::pair<std::string, std::string> materialNames_;
  std::pair<std::unique_ptr<Ogre::Rectangle2D>, std::unique_ptr<Ogre::Rectangle2D>> rectangles_;
};

#endif  // DYNAMIC_ASSETS_VIDEOPLAYERENTITY_H
