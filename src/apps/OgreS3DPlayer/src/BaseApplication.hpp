#ifndef __BaseApplication_hpp_
#define __BaseApplication_hpp_

#include <OgreWindowEventUtilities.h>
#include <OgreFrameListener.h>
#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreFileSystemLayer.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include "CameraMan.hpp"

class BaseApplication : public Ogre::WindowEventListener, public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener
{
public:
    BaseApplication();
    virtual ~BaseApplication();

    virtual void run();

protected:
    virtual bool setup();
    virtual bool configure();
    virtual void chooseSceneManager();
    virtual void createCamera();
    virtual void createFrameListener();
    virtual void createScene() = 0; // Override me!
    virtual void destroyScene();
    virtual void createViewports();
    virtual void setupResources();
    virtual void createResourceListener();
    virtual void loadResources();

    // Ogre::FrameListener
    virtual bool frameRenderingQueued(const Ogre::FrameEvent &);

    // OIS::KeyListener
    virtual bool keyPressed(const OIS::KeyEvent &);
    virtual bool keyReleased(const OIS::KeyEvent &);
    // OIS::MouseListener
    virtual bool mouseMoved(const OIS::MouseEvent &);
    virtual bool mousePressed(const OIS::MouseEvent &, OIS::MouseButtonID);
    virtual bool mouseReleased(const OIS::MouseEvent &, OIS::MouseButtonID);

    // Ogre::WindowEventListener
    virtual void windowResized(Ogre::RenderWindow *); //Adjust mouse clipping area
    virtual void windowClosed(Ogre::RenderWindow *); //Unattach OIS before window shutdown (very important under Linux)

    std::unique_ptr<Ogre::FileSystemLayer, void(*)(Ogre::FileSystemLayer*)> mFSLayer; // File system abstraction layer
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;

    std::unique_ptr<Ogre::Root> mRoot;
    Ogre::Camera *mCamera;
    Ogre::SceneManager *mSceneMgr;
    Ogre::RenderWindow *mWindow;
	char mPolygonRenderingMode;
    bool mShutDown;

    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse*    mMouse;
    OIS::Keyboard* mKeyboard;

    // OgreCookies
    std::unique_ptr<OgreCookies::CameraMan> mCameraMan;       // basic camera controller
};

#endif // #ifndef __BaseApplication_h_
