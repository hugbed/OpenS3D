//
// Created by jon on 13/04/17.
//

#ifndef DYNAMIC_ASSETS_RECTANGLEFACTORY_H
#define DYNAMIC_ASSETS_RECTANGLEFACTORY_H

#include <OgreRectangle2D.h>

// todo:
namespace RectangleFactory {

inline std::unique_ptr<Ogre::Rectangle2D> createRectangle(std::pair<float, float> boundaries) {
  auto rect = std::make_unique<Ogre::Rectangle2D>(true);
  rect->setCorners(boundaries.first, 1.0f, boundaries.second, -1.0f);

  // Render the background before everything else
  rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);

  // Use infinite AAB to always stay visible
  Ogre::AxisAlignedBox aabInf;
  aabInf.setInfinite();
  rect->setBoundingBox(aabInf);

  return rect;
}

inline std::unique_ptr<Ogre::Rectangle2D> createRectangle(std::pair<float, float> boundaries,
                                                          const std::string& materialName) {
  auto rect = createRectangle(boundaries);
  rect->setMaterial(materialName);
  return rect;
}
}  // namespace RectangleFactory

#endif  // DYNAMIC_ASSETS_RECTANGLEFACTORY_H
