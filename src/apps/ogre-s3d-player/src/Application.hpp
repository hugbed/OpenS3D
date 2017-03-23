#ifndef __Application_hpp_
#define __Application_hpp_

#include "BaseApplication.hpp"

#include <thread>

class DynamicImageTexture;
class VideoTexture;

class Application : public BaseApplication
{
public:
	Application();
    virtual ~Application();

    void loadImagesThread();

protected:

    virtual bool frameRenderingQueued(const Ogre::FrameEvent &) override;

    void createScene();

    // to stream image to texture
    std::unique_ptr<VideoTexture> m_videoTexture;
    std::unique_ptr<VideoTexture> m_videoTextureR;

    // image loading
    std::unique_ptr<std::thread> m_pImageThread;

    // frame request listeners
    std::vector<Ogre::FrameListener*> m_frameListeners;
};

#endif // #ifndef __Application_h_
