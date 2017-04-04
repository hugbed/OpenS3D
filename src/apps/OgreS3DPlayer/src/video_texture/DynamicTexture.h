//
// Created by jon on 18/03/17.
//

#ifndef OGRE_SAMPLE_DYNAMICMATERIAL_H
#define OGRE_SAMPLE_DYNAMICMATERIAL_H

#include <Ogre.h>

#include <vector>

class DynamicTexture {
public:
    DynamicTexture(const std::string &textureName,
                    Ogre::PixelFormat format,
                    ushort imgWidth,
                    ushort imgHeight)
        : m_textureName{ textureName }
        , m_imgWidth{ imgWidth }
        , m_imgHeight{ imgHeight }
    {
        m_pDynamicTexture = Ogre::TextureManager::getSingleton().createManual(m_textureName,
                                                                              Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                                              Ogre::TEX_TYPE_2D,
                                                                              m_imgWidth, m_imgHeight, // w, h
                                                                              0,
                                                                              Ogre::PF_R8G8B8,
                                                                              Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
        clear();
    }

    // source: http://ogre3d.org/tikiwiki/Creating+dynamic+textures
    virtual void updateImage(const std::vector<uint8_t> &data)
    {
        auto pixelBuffer = m_pDynamicTexture->getBuffer();
        assert(data.size() == pixelBuffer->getSizeInBytes());

        pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

        const auto& pixelBox = pixelBuffer->getCurrentLock();
        std::copy(std::begin(data), std::end(data), static_cast<uint8_t*>(pixelBox.data));

        pixelBuffer->unlock();
    }

    void clear()
    {
        std::vector<unsigned char> imgData;
        imgData.resize(m_pDynamicTexture->getBuffer()->getSizeInBytes());
        std::fill(std::begin(imgData), std::end(imgData), 0x00); // all black
        updateImage(imgData);
    }

    size_t getSizeInBytes() const
    {
        return m_pDynamicTexture->getBuffer()->getSizeInBytes(); // could be nullptr?
    }

    const std::string& getTextureName()
    {
        return m_textureName;
    }

    static Ogre::MaterialPtr createImageMaterial(const std::string &materialName, const std::string &textureName)
    {
        Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(materialName, "General");
        material->getTechnique(0)->getPass(0)->createTextureUnitState(textureName);
        material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
        material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
        material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
        return material;
    }

private:
    std::string m_textureName;

    ushort m_imgWidth;
    ushort m_imgHeight;

    Ogre::SharedPtr<Ogre::Texture> m_pDynamicTexture;
};


#endif //OGRE_SAMPLE_DYNAMICMATERIAL_H
