#include "Application.hpp"
#include "DynamicImageTexture.h"

#include <OgreSceneManager.h>
#include <OgreViewport.h>
#include <OgreCamera.h>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreColourValue.h>
#include <OgreException.h>
#include <OISException.h>

#include <OgreRectangle2D.h>
#include "VideoTexture.h"

#include "YUVToRGBFileBytesProducer.h"
#include "VideoConsumer.h"

#include <chrono>
#include <future>

//-------------------------------------------------------------------------------------
Application::Application()
    : m_pImageThread{}
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
    // Create background material
    {
        // solution is dynamic texture
        // see http://ogre3d.org/tikiwiki/Creating+dynamic+textures
        auto dynamicTexture = std::make_unique<DynamicImageTexture>("DynamicTextureL", Ogre::PixelFormat::PF_R8G8B8, 1920,
                                                                 1080);
        auto material = DynamicImageTexture::createImageMaterial("BackgroundMaterialL",
                                                                 dynamicTexture->getTextureName());
        m_videoTexture = std::make_unique<VideoTexture>(std::move(dynamicTexture), 1.0f/60.0f);
        m_frameListeners.push_back(m_videoTexture.get());

        // Create background rectangle covering the whole screen
        Ogre::Rectangle2D *rect = new Ogre::Rectangle2D(true);
        rect->setCorners(-1.0f, 1.0f, 0.0f, -1.0f);
        rect->setMaterial(material->getName());

        // Render the background before everything else
        rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);

        // Use infinite AAB to always stay visible
        Ogre::AxisAlignedBox aabInf;
        aabInf.setInfinite();
        rect->setBoundingBox(aabInf);

        // Attach background to the scene
        Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("BackgroundL");
        node->attachObject(rect);
    }

    // Create background material
    {
        // solution is dynamic texture
        // see http://ogre3d.org/tikiwiki/Creating+dynamic+textures
        auto dynamicTexture = std::make_unique<DynamicImageTexture>("DynamicTextureR", Ogre::PixelFormat::PF_R8G8B8, 1920,
                                                                 1080);
        auto material = DynamicImageTexture::createImageMaterial("BackgroundMaterialR",
                                                                 dynamicTexture->getTextureName());
        m_videoTextureR = std::make_unique<VideoTexture>(std::move(dynamicTexture), 1.0f/60.0f);
        m_frameListeners.push_back(m_videoTextureR.get());

        // Create background rectangle covering the whole screen
        Ogre::Rectangle2D *rect = new Ogre::Rectangle2D(true);
        rect->setCorners(0.0f, 1.0f, 1.0f, -1.0f);
        rect->setMaterial(material->getName());

        // Render the background before everything else
        rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);

        // Use infinite AAB to always stay visible
        Ogre::AxisAlignedBox aabInf;
        aabInf.setInfinite();
        rect->setBoundingBox(aabInf);

        // Attach background to the scene
        Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("BackgroundR");
        node->attachObject(rect);
    }

    m_pImageThread = std::make_unique<std::thread>(&Application::loadImagesThread, this);
    m_pImageThread->detach();

// Don't forget to delete the Rectangle2D in the destructor of your application:
//    delete rect;
}

void Application::loadImagesThread()
{
    using namespace std::chrono;
    using namespace std::chrono_literals;

    std::mutex doneProducingMutex;
    std::condition_variable shouldConsumeCV;

    YUVToRGBFileBytesProducer fileProducerL("../media/materials/textures/current-left.yuv", doneProducingMutex, shouldConsumeCV);
    YUVToRGBFileBytesProducer fileProducerR("../media/materials/textures/current-right.yuv", doneProducingMutex, shouldConsumeCV);

    std::vector<SyncProducer<std::vector<uint8_t>>*> producers = {
        &fileProducerL,
        &fileProducerR
    };

    auto pL_thread = std::thread([&fileProducerL]{
        fileProducerL.startProducing();
    });

    auto pR_thread = std::thread([&fileProducerR]{
        fileProducerR.startProducing();
    });

    VideoConsumer consumer(m_videoTexture.get(), m_videoTextureR.get(), doneProducingMutex, shouldConsumeCV, producers);
    consumer.startConsuming();

    pL_thread.join();
    pR_thread.join();
}
