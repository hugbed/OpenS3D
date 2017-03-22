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
#include "DynamicImageTexture.h"
#include "VideoTexture.h"

#include "file_io.h"
#include "compression.h"

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
    // should really use a list of frameRenderingQueueListeners
    if (m_videoTexture) m_videoTexture->frameRenderingQueued(evt);
    if (m_videoTextureR) m_videoTextureR->frameRenderingQueued(evt);
    return BaseApplication::frameRenderingQueued(evt);
}

//-------------------------------------------------------------------------------------
void Application::createScene() {
    // Create background material
    {
        // solution is dynamic texture
        // see http://ogre3d.org/tikiwiki/Creating+dynamic+textures
        m_dynamicTexture = std::make_unique<DynamicImageTexture>("DynamicTextureL", Ogre::PixelFormat::PF_R8G8B8, 1920,
                                                                 1080);
        auto material = DynamicImageTexture::createImageMaterial("BackgroundMaterialL",
                                                                 m_dynamicTexture->getTextureName());
        m_videoTexture = std::make_unique<VideoTexture>(m_dynamicTexture.get(), 1.0f/60.0f);

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
        m_dynamicTextureR = std::make_unique<DynamicImageTexture>("DynamicTextureR", Ogre::PixelFormat::PF_R8G8B8, 1920,
                                                                 1080);
        auto material = DynamicImageTexture::createImageMaterial("BackgroundMaterialR",
                                                                 m_dynamicTextureR->getTextureName());
        m_videoTextureR = std::make_unique<VideoTexture>(m_dynamicTextureR.get(), 1.0f/60.0f);

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

    const auto frameSizeBytesYUV = 1920 * 1080 * 2;
    const auto frameSizeBytesRGB = 1920 * 1080 * 3;

    std::ifstream videoFile{"../media/materials/textures/current.yuv", std::ios::binary};

    std::vector<uint8_t> fileDataL, fileDataR, rgbDataL, rgbDataR;
    fileDataL.resize(frameSizeBytesYUV);
    fileDataR.resize(frameSizeBytesYUV);
    rgbDataL.resize(frameSizeBytesRGB);
    rgbDataR.resize(frameSizeBytesRGB);

    for (int i = 0; i < 100; ++i) {

        auto t1 = std::chrono::high_resolution_clock::now();

        // LEFT FRAME
        read_n_bytes(videoFile, frameSizeBytesYUV, std::begin(fileDataL));
        assert(fileDataL.size() == frameSizeBytesYUV);
        // could be interface to some decompression algorithm : like decompress()
        // RIGHT FRAME
        read_n_bytes(videoFile, frameSizeBytesYUV, std::begin(fileDataR));
        assert(fileDataR.size() == frameSizeBytesYUV);

        // could be interface to some decompression algorithm : like decompress()
        yuv2rgb(std::begin(fileDataL), std::end(fileDataL), std::begin(rgbDataL));
        assert(rgbDataL.size() == frameSizeBytesRGB);
        yuv2rgb(std::begin(fileDataR), std::end(fileDataR), std::begin(rgbDataR));
        assert(rgbDataR.size() == frameSizeBytesRGB);

        m_videoTexture->pushFrame(rgbDataL);
        m_videoTextureR->pushFrame(rgbDataR);

        std::this_thread::sleep_for(33ms - duration_cast<milliseconds>(high_resolution_clock::now()-t1));

//        t2 = std::chrono::high_resolution_clock::now();
//        std::cout << "Loop time : " << duration_cast<milliseconds>(t2-t1).count() << std::endl;
    }
}
