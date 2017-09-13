#ifndef VIDEO_TEXTURE_DYNAMICTEXTURE_H
#define VIDEO_TEXTURE_DYNAMICTEXTURE_H

#include <Ogre.h>

#include "gsl/gsl"

#include <vector>

class DynamicTexture {
 public:
  DynamicTexture(std::string textureName,
                 Ogre::PixelFormat format,
                 ushort imgWidth,
                 ushort imgHeight)
      : m_textureName{std::move(textureName)} {
    m_pDynamicTexture = Ogre::TextureManager::getSingleton().createManual(
        m_textureName,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        Ogre::TEX_TYPE_2D,
        imgWidth,
        imgHeight,  // w, h
        0,
        format,
        Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
    clear();
  }

  // source: http://ogre3d.org/tikiwiki/Creating+dynamic+textures
  virtual void updateImage(gsl::span<const uint8_t> imageBytes) {
    auto pixelBuffer = m_pDynamicTexture->getBuffer();
    assert(imageBytes.size() == pixelBuffer->getSizeInBytes());

    pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

    const auto& pixelBox = pixelBuffer->getCurrentLock();
    std::copy(std::begin(imageBytes), std::end(imageBytes), static_cast<uint8_t*>(pixelBox.data));

    pixelBuffer->unlock();
  }

  void clear() {
    std::vector<unsigned char> imgData;
    imgData.resize(m_pDynamicTexture->getBuffer()->getSizeInBytes());
    std::fill(std::begin(imgData), std::end(imgData), 0x00);  // all black
    updateImage(imgData);
  }

  size_t getSizeInBytes() const {
    return m_pDynamicTexture->getBuffer()->getSizeInBytes();  // could be nullptr?
  }

  const std::string& getTextureName() { return m_textureName; }

  static Ogre::MaterialPtr createImageMaterial(const std::string& materialName,
                                               const std::string& textureName) {
    Ogre::MaterialPtr material =
        Ogre::MaterialManager::getSingleton().create(materialName, "General");
    auto textureUnit = material->getTechnique(0)->getPass(0)->createTextureUnitState(textureName);
    textureUnit->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
    textureUnit->setTextureBorderColour(Ogre::ColourValue::Black);
    material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
    material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
    material->getTechnique(0)->getPass(0)->setLightingEnabled(false);

    return material;
  }

 private:
  std::string m_textureName;

  Ogre::SharedPtr<Ogre::Texture> m_pDynamicTexture;
};

#endif  // VIDEO_TEXTURE_DYNAMICTEXTURE_H
