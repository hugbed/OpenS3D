#ifndef BASEAPPLICATION_HPP
#define BASEAPPLICATION_HPP

#include <OgreCamera.h>
#include <OgreConfigFile.h>
#include <OgreEntity.h>
#include <OgreFileSystemLayer.h>
#include <OgreFrameListener.h>
#include <OgreLogManager.h>
#include <OgreRenderWindow.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreViewport.h>
#include <OgreWindowEventUtilities.h>

#include "CameraMan.hpp"
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

class BaseApplication : public Ogre::WindowEventListener,
                        public Ogre::FrameListener,
                        public OIS::KeyListener,
                        public OIS::MouseListener {
 public:
  BaseApplication();
  ~BaseApplication() override;

  // rule of five, not logical for this class to be copied or moved
  BaseApplication(const BaseApplication&) = delete;
  BaseApplication& operator=(const BaseApplication&) = delete;
  BaseApplication(BaseApplication&& /*unused*/) = delete;
  BaseApplication& operator=(BaseApplication&& /*unused*/) = delete;

  virtual void run();

 protected:
  virtual bool setup();
  virtual bool configure();
  virtual void chooseSceneManager();
  virtual void createCamera();
  virtual void createFrameListener();
  virtual void createScene() = 0;  // Override me!
  virtual void destroyScene();
  virtual void createViewports();
  virtual void setupResources();
  virtual void createResourceListener();
  virtual void loadResources();

  // Ogre::FrameListener
  bool frameRenderingQueued(const Ogre::FrameEvent& /*evt*/) override;

  // OIS::KeyListener
  bool keyPressed(const OIS::KeyEvent& /*arg*/) override;
  bool keyReleased(const OIS::KeyEvent& /*arg*/) override;
  // OIS::MouseListener
  bool mouseMoved(const OIS::MouseEvent& /*arg*/) override;
  bool mousePressed(const OIS::MouseEvent& /*arg*/, OIS::MouseButtonID /*id*/) override;
  bool mouseReleased(const OIS::MouseEvent& /*arg*/, OIS::MouseButtonID /*id*/) override;

  // Ogre::WindowEventListener
  void windowResized(Ogre::RenderWindow* /*rw*/) override;  // Adjust mouse clipping
                                                            // area
  void windowClosed(Ogre::RenderWindow* /*rw*/) override;   // Unattach OIS before window
                                                            // shutdown (very important
                                                            // under Linux)

  std::unique_ptr<Ogre::FileSystemLayer, void (*)(Ogre::FileSystemLayer*)>
      mFSLayer;  // File system abstraction layer
  Ogre::String mResourcesCfg{Ogre::BLANKSTRING};
  Ogre::String mPluginsCfg{Ogre::BLANKSTRING};

  std::unique_ptr<Ogre::Root> mRoot;
  Ogre::Camera* mCamera{};
  Ogre::SceneManager* mSceneMgr{};
  Ogre::RenderWindow* mWindow{};
  char mPolygonRenderingMode{'B'};
  bool mShutDown{false};

  // OIS Input devices
  OIS::InputManager* mInputManager{};
  OIS::Mouse* mMouse{};
  OIS::Keyboard* mKeyboard{};

  // OgreCookies
  std::unique_ptr<OgreCookies::CameraMan> mCameraMan;  // basic camera controller
};

#endif  // BASEAPPLICATION_HPP
