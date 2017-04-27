#ifndef DYNAMIC_ASSETS_VIDEOPLAYERSBSENTITY_H
#define DYNAMIC_ASSETS_VIDEOPLAYERSBSENTITY_H

#include "SplitscreenRectangleEntity.hpp"

class VideoPlayerSBSEntity : public SplitscreenRectangleEntity {
 public:
  VideoPlayerSBSEntity(std::string entityName,
                       const std::string& textureNameLeft,
                       const std::string& textureNameRight)
      : SplitscreenRectangleEntity(std::move(entityName)) {
    auto materialLeft = static_cast<Ogre::MaterialPtr>(
        Ogre::MaterialManager::getSingleton().getByName("StereoSide", "General"));
    materialLeft->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
    materialLeft->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
    materialLeft->getTechnique(0)->getPass(0)->setLightingEnabled(false);

    auto textureUnitLeft =
        materialLeft->getTechnique(0)->getPass(0)->createTextureUnitState(textureNameLeft);
    textureUnitLeft->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
    textureUnitLeft->setTextureBorderColour(Ogre::ColourValue::Black);

    auto materialRight = materialLeft->clone("StereoSideRight");

    auto textureUnitRight =
        materialRight->getTechnique(0)->getPass(0)->createTextureUnitState(textureNameRight);
    textureUnitRight->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
    textureUnitRight->setTextureBorderColour(Ogre::ColourValue::Black);

    constexpr Ogre::Real horizontalShift = 0.015;

    materialLeft->getTechnique(0)->getPass(0)->getVertexProgramParameters()->setNamedConstant(
        "horizontalShift", horizontalShift);
    materialRight->getTechnique(0)->getPass(0)->getVertexProgramParameters()->setNamedConstant(
        "horizontalShift", -horizontalShift);

    setMaterialNames(materialLeft->getName(), materialRight->getName());
  }
};

#endif  // DYNAMIC_ASSETS_VIDEOPLAYERSBSENTITY_H
