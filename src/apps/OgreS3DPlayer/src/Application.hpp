#ifndef __Application_hpp_
#define __Application_hpp_

#include "BaseApplication.hpp"

#include <thread>

class DynamicTexture;
class DynamicTextureThreadSafe;
class TextureUpdateManager;

class Application : public BaseApplication
{
public:
	Application();
    virtual ~Application();

protected:

    virtual bool frameRenderingQueued(const Ogre::FrameEvent &) override;

    void createScene();

    void addLights();

    void createGroundPlane();

    void createVideoRectangle(
        std::unique_ptr<Ogre::Rectangle2D> &rect,
        std::unique_ptr<DynamicTextureThreadSafe> &videoTexture,
        const std::string &id
    );

    void createVideoPlane(std::unique_ptr<DynamicTextureThreadSafe> &videoTexture, const std::string &id);

    void createPointCloud();

    // to stream image to texture (L, R)
    std::pair<std::unique_ptr<DynamicTextureThreadSafe>, std::unique_ptr<DynamicTextureThreadSafe>> m_videoTextures;
    std::pair<std::unique_ptr<Ogre::Rectangle2D>, std::unique_ptr<Ogre::Rectangle2D>> m_Rectangles;

    std::unique_ptr<TextureUpdateManager> m_textureUpdateManager;

    // frame request listeners
    std::vector<Ogre::FrameListener*> m_frameListeners;
};

#endif // #ifndef __Application_h_
