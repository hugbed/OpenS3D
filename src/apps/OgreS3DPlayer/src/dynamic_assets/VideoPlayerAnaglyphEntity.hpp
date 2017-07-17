#ifndef DYNAMIC_ASSETS_VIDEOPLAYERANAGLYPHENTITY_H
#define DYNAMIC_ASSETS_VIDEOPLAYERANAGLYPHENTITY_H

#include "FullscreenRectangleEntity.hpp"

class VideoPlayerAnaglyphEntity : public FullscreenRectangleEntity {
 public:
  VideoPlayerAnaglyphEntity(const std::string& entityName,
                            const std::string& textureNameLeft,
                            const std::string& textureNameRight)
      : FullscreenRectangleEntity(entityName) {
    // anaglyph material
    auto material = static_cast<Ogre::MaterialPtr>(
        Ogre::MaterialManager::getSingleton().getByName("Anaglyph", "General"));
    auto textureUnit =
        material->getTechnique(0)->getPass(0)->createTextureUnitState(textureNameLeft);
    textureUnit->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
    textureUnit->setTextureBorderColour(Ogre::ColourValue::Black);
    textureUnit = material->getTechnique(0)->getPass(0)->createTextureUnitState(textureNameRight);
    textureUnit->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
    textureUnit->setTextureBorderColour(Ogre::ColourValue::Black);
    material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
    material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
    material->getTechnique(0)->getPass(0)->setLightingEnabled(false);

    // set horizontal shift (%)
    material->getTechnique(0)->getPass(0)->getVertexProgramParameters()->setNamedConstant(
        "horizontalShift", Ogre::Real(0.00));

    setMaterialName(material->getName());

    // this must be added to the scene now
  }
};

#endif  // DYNAMIC_ASSETS_VIDEOPLAYERANAGLYPHENTITY_H
