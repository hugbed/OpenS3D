#ifndef CAMERAMAN_HPP
#define CAMERAMAN_HPP

#include <Ogre.h>
#include <OgreVector3.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <limits>

namespace OgreCookies {
enum CameraStyle  // enumerator values for different styles of camera movement
{ CS_FREELOOK,
  CS_ORBIT,
  CS_MANUAL };

/*=============================================================================
| Utility class for controlling the camera in samples.
=============================================================================*/
class CameraMan {
 public:
  explicit CameraMan(Ogre::Camera* cam);

  // rule of five, this class should not be copied
  CameraMan(const CameraMan&) = delete;
  CameraMan& operator=(const CameraMan&) = delete;
  CameraMan(CameraMan&& /*unused*/) noexcept;
  CameraMan& operator=(CameraMan&& /*unused*/) noexcept;

  /*-----------------------------------------------------------------------------
  | Swaps the camera on our camera man for another camera.
  -----------------------------------------------------------------------------*/
  virtual void setCamera(Ogre::Camera* cam);
  virtual Ogre::Camera* getCamera();

  /*-----------------------------------------------------------------------------
  | Sets the target we will revolve around. Only applies for orbit style.
  -----------------------------------------------------------------------------*/
  virtual void setTarget(Ogre::SceneNode* target);
  virtual Ogre::SceneNode* getTarget();

  /*-----------------------------------------------------------------------------
  | Sets the spatial offset from the target. Only applies for orbit style.
  -----------------------------------------------------------------------------*/
  virtual void setYawPitchDist(Ogre::Radian yaw,
                               Ogre::Radian pitch,
                               Ogre::Real dist);

  /*-----------------------------------------------------------------------------
  | Sets the camera's top speed. Only applies for free-look style.
  -----------------------------------------------------------------------------*/
  virtual void setTopSpeed(Ogre::Real topSpeed);
  virtual Ogre::Real getTopSpeed();

  /*-----------------------------------------------------------------------------
  | Sets the movement style of our camera man.
  -----------------------------------------------------------------------------*/
  virtual void setStyle(CameraStyle style);
  virtual CameraStyle getStyle();

  /*-----------------------------------------------------------------------------
  | Manually stops the camera when in free-look mode.
  -----------------------------------------------------------------------------*/
  virtual void manualStop();
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

  /*-----------------------------------------------------------------------------
  | Processes key presses for free-look style movement.
  -----------------------------------------------------------------------------*/
  virtual void injectKeyDown(const OIS::KeyEvent& evt);

  /*-----------------------------------------------------------------------------
  | Processes key releases for free-look style movement.
  -----------------------------------------------------------------------------*/
  virtual void injectKeyUp(const OIS::KeyEvent& evt);

/*-----------------------------------------------------------------------------
| Processes mouse movement differently for each style.
-----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || \
    (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
  virtual void injectMouseMove(const OIS::MultiTouchEvent& evt);
#else
  virtual void injectMouseMove(const OIS::MouseEvent& evt);
#endif

/*-----------------------------------------------------------------------------
| Processes mouse presses. Only applies for orbit style.
| Left button is for orbiting, and right button is for zooming.
-----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || \
    (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
  virtual void injectMouseDown(const OIS::MultiTouchEvent& evt);
#else
  virtual void injectMouseDown(const OIS::MouseEvent& evt,
                               OIS::MouseButtonID id);
#endif

/*-----------------------------------------------------------------------------
| Processes mouse releases. Only applies for orbit style.
| Left button is for orbiting, and right button is for zooming.
-----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || \
    (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
  virtual void injectMouseUp(const OIS::MultiTouchEvent& evt);
#else
  virtual void injectMouseUp(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
#endif

 protected:
  Ogre::Camera* mCamera{};
  CameraStyle mStyle{};
  Ogre::SceneNode* mTarget{};
  bool mOrbiting{false};
  bool mZooming{false};
  Ogre::Real mTopSpeed{0};
  Ogre::Vector3 mVelocity{0, 0, 0};
  bool mGoingForward{false};
  bool mGoingBack{false};
  bool mGoingLeft{false};
  bool mGoingRight{false};
  bool mGoingUp{false};
  bool mGoingDown{false};
  bool mFastMove{false};
};
}  // namespace OgreCookies

#endif  // CAMERAMAN_HPP
