#include "Application.hpp"
#include "video_texture/DynamicTexture.h"

#include <OgreSceneManager.h>
#include <OgreViewport.h>
#include <OgreCamera.h>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreColourValue.h>
#include <OgreException.h>
#include <OISException.h>

#include <OgreRectangle2D.h>
#include "video_texture/DynamicTextureThreadSafe.h"

#include "video_texture/IfYUVToRGBProducer.h"
#include "video_texture/S3DVideoRGBConsumer.h"
#include "video_texture/TextureUpdateManager.h"

#include <future>

//-------------------------------------------------------------------------------------
Application::Application()
    : m_textureUpdateManager(
        std::make_unique<YUVFileTextureUpdateManager>("../media/materials/textures/current-left.yuv",
                                                      "../media/materials/textures/current-right.yuv"))
{
}

//-------------------------------------------------------------------------------------
Application::~Application()
{
}

bool Application::frameRenderingQueued(const Ogre::FrameEvent & evt)
{
    for (auto & listeners : m_frameListeners)
        listeners->frameRenderingQueued(evt);
    return BaseApplication::frameRenderingQueued(evt);
}

//-------------------------------------------------------------------------------------
void Application::createScene() {
    createVideoRectangle(m_Rectangles.first, m_videoTextures.first, "L");
    createVideoRectangle(m_Rectangles.second, m_videoTextures.second, "R");

    // start texture update thread or something
    m_textureUpdateManager->handleTextureUpdate(m_videoTextures.first.get(), m_videoTextures.second.get());
}

void Application::createVideoRectangle(
    std::unique_ptr<Ogre::Rectangle2D> &rect,
    std::unique_ptr<DynamicTextureThreadSafe> &videoTexture,
    const std::string &id
)
{
    constexpr auto refreshRate = 1.0f/60.0f; // twice as fast as update to prevent aliasing (if this makes sense)

    // Using: http://ogre3d.org/tikiwiki/Creating+dynamic+textures
    videoTexture = std::make_unique<DynamicTextureThreadSafe>(std::string("DynamicTexture").append(id),
                                                              Ogre::PixelFormat::PF_R8G8B8, 1920, 1080, refreshRate);
    auto material = DynamicTexture::createImageMaterial(std::string("BackgroundMaterial").append(id),
                                                        videoTexture->getTextureName());

    m_frameListeners.push_back(videoTexture.get());

    // Create background rectangles covering the whole screen
    std::pair<float, float> boundaries{-1.0f, 0.0f};
    if (id == "R") {
        boundaries = {0.0f, 1.0f};
    }
    rect = std::make_unique<Ogre::Rectangle2D>(true);
    rect->setCorners(boundaries.first, 1.0f, boundaries.second, -1.0f);
    rect->setMaterial(material->getName());

    // Render the background before everything else
    rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);

    // Use infinite AAB to always stay visible
    Ogre::AxisAlignedBox aabInf;
    aabInf.setInfinite();
    rect->setBoundingBox(aabInf);

    // Attach background to the scene
    Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode(std::string("Background").append(id));
    node->attachObject(rect.get());
}