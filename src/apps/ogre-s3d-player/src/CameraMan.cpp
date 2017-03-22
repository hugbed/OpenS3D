#include "CameraMan.hpp"

using namespace OgreCookies;

//-------------------------------------------------------------------------------------
CameraMan::CameraMan(Ogre::Camera* cam)
    : mCamera(0)
    , mTarget(0)
    , mOrbiting(false)
    , mZooming(false)
    , mTopSpeed(150)
    , mVelocity(Ogre::Vector3::ZERO)
    , mGoingForward(false)
    , mGoingBack(false)
    , mGoingLeft(false)
    , mGoingRight(false)
    , mGoingUp(false)
    , mGoingDown(false)
    , mFastMove(false)
{
    setCamera(cam);
    setStyle(CS_FREELOOK);
}

CameraMan::~CameraMan()
{

}

/*-----------------------------------------------------------------------------
| Swaps the camera on our camera man for another camera.
-----------------------------------------------------------------------------*/
void CameraMan::setCamera(Ogre::Camera* cam)
{
    mCamera = cam;
}

Ogre::Camera* CameraMan::getCamera()
{
    return mCamera;
}

/*-----------------------------------------------------------------------------
| Sets the target we will revolve around. Only applies for orbit style.
-----------------------------------------------------------------------------*/
void CameraMan::setTarget(Ogre::SceneNode* target)
{
    if (target != mTarget)
    {
        mTarget = target;
        if(target)
        {
            setYawPitchDist(Ogre::Degree(0), Ogre::Degree(15), 150);
            mCamera->setAutoTracking(true, mTarget);
        }
        else
        {
            mCamera->setAutoTracking(false);
        }

    }


}

Ogre::SceneNode* CameraMan::getTarget()
{
    return mTarget;
}

/*-----------------------------------------------------------------------------
| Sets the spatial offset from the target. Only applies for orbit style.
-----------------------------------------------------------------------------*/
void CameraMan::setYawPitchDist(Ogre::Radian yaw, Ogre::Radian pitch, Ogre::Real dist)
{
    mCamera->setPosition(mTarget->_getDerivedPosition());
    mCamera->setOrientation(mTarget->_getDerivedOrientation());
    mCamera->yaw(yaw);
    mCamera->pitch(-pitch);
    mCamera->moveRelative(Ogre::Vector3(0, 0, dist));
}

/*-----------------------------------------------------------------------------
| Sets the camera's top speed. Only applies for free-look style.
-----------------------------------------------------------------------------*/
void CameraMan::setTopSpeed(Ogre::Real topSpeed)
{
    mTopSpeed = topSpeed;
}

Ogre::Real CameraMan::getTopSpeed()
{
    return mTopSpeed;
}

/*-----------------------------------------------------------------------------
| Sets the movement style of our camera man.
-----------------------------------------------------------------------------*/
void CameraMan::setStyle(CameraStyle style)
{
    if (mStyle != CS_ORBIT && style == CS_ORBIT)
    {
        setTarget(mTarget ? mTarget : mCamera->getSceneManager()->getRootSceneNode());
        mCamera->setFixedYawAxis(true);
        manualStop();
        setYawPitchDist(Ogre::Degree(0), Ogre::Degree(15), 150);
    }
    else if (mStyle != CS_FREELOOK && style == CS_FREELOOK)
    {
        mCamera->setAutoTracking(false);
        mCamera->setFixedYawAxis(true);
    }
    else if (mStyle != CS_MANUAL && style == CS_MANUAL)
    {
        mCamera->setAutoTracking(false);
        manualStop();
    }
    mStyle = style;

}

CameraStyle CameraMan::getStyle()
{
    return mStyle;
}

/*-----------------------------------------------------------------------------
| Manually stops the camera when in free-look mode.
-----------------------------------------------------------------------------*/
void CameraMan::manualStop()
{
    if (mStyle == CS_FREELOOK)
    {
        mGoingForward = false;
        mGoingBack = false;
        mGoingLeft = false;
        mGoingRight = false;
        mGoingUp = false;
        mGoingDown = false;
        mVelocity = Ogre::Vector3::ZERO;
    }
}

bool CameraMan::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if (mStyle == CS_FREELOOK)
    {
        // build our acceleration vector based on keyboard input composite
        Ogre::Vector3 accel = Ogre::Vector3::ZERO;
        if (mGoingForward) accel += mCamera->getDirection();
        if (mGoingBack) accel -= mCamera->getDirection();
        if (mGoingRight) accel += mCamera->getRight();
        if (mGoingLeft) accel -= mCamera->getRight();
        if (mGoingUp) accel += mCamera->getUp();
        if (mGoingDown) accel -= mCamera->getUp();

        // if accelerating, try to reach top speed in a certain time
        Ogre::Real topSpeed = mFastMove ? mTopSpeed * 20 : mTopSpeed;
        if (accel.squaredLength() != 0)
        {
            accel.normalise();
            mVelocity += accel * topSpeed * evt.timeSinceLastFrame * 10;
        }
        // if not accelerating, try to stop in a certain time
        else mVelocity -= mVelocity * evt.timeSinceLastFrame * 10;

        Ogre::Real tooSmall = std::numeric_limits<Ogre::Real>::epsilon();

        // keep camera velocity below top speed and above epsilon
        if (mVelocity.squaredLength() > topSpeed * topSpeed)
        {
            mVelocity.normalise();
            mVelocity *= topSpeed;
        }
        else if (mVelocity.squaredLength() < tooSmall * tooSmall)
            mVelocity = Ogre::Vector3::ZERO;

        if (mVelocity != Ogre::Vector3::ZERO) mCamera->move(mVelocity * evt.timeSinceLastFrame);
    }

    return true;
}

/*-----------------------------------------------------------------------------
| Processes key presses for free-look style movement.
-----------------------------------------------------------------------------*/
void CameraMan::injectKeyDown(const OIS::KeyEvent& evt)
{
    if (mStyle == CS_FREELOOK)
    {
        if (evt.key == OIS::KC_W || evt.key == OIS::KC_UP) mGoingForward = true;
        else if (evt.key == OIS::KC_S || evt.key == OIS::KC_DOWN) mGoingBack = true;
        else if (evt.key == OIS::KC_A || evt.key == OIS::KC_LEFT) mGoingLeft = true;
        else if (evt.key == OIS::KC_D || evt.key == OIS::KC_RIGHT) mGoingRight = true;
        else if (evt.key == OIS::KC_PGUP) mGoingUp = true;
        else if (evt.key == OIS::KC_PGDOWN) mGoingDown = true;
        else if (evt.key == OIS::KC_LSHIFT) mFastMove = true;
    }
}

/*-----------------------------------------------------------------------------
| Processes key releases for free-look style movement.
-----------------------------------------------------------------------------*/
void CameraMan::injectKeyUp(const OIS::KeyEvent& evt)
{
    if (mStyle == CS_FREELOOK)
    {
        if (evt.key == OIS::KC_W || evt.key == OIS::KC_UP) mGoingForward = false;
        else if (evt.key == OIS::KC_S || evt.key == OIS::KC_DOWN) mGoingBack = false;
        else if (evt.key == OIS::KC_A || evt.key == OIS::KC_LEFT) mGoingLeft = false;
        else if (evt.key == OIS::KC_D || evt.key == OIS::KC_RIGHT) mGoingRight = false;
        else if (evt.key == OIS::KC_PGUP) mGoingUp = false;
        else if (evt.key == OIS::KC_PGDOWN) mGoingDown = false;
        else if (evt.key == OIS::KC_LSHIFT) mFastMove = false;
    }
}

/*-----------------------------------------------------------------------------
| Processes mouse movement differently for each style.
-----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
void CameraMan::injectMouseMove(const OIS::MultiTouchEvent& evt)
#else
void CameraMan::injectMouseMove(const OIS::MouseEvent& evt)
#endif
{
    if (mStyle == CS_ORBIT)
    {
        Ogre::Real dist = (mCamera->getPosition() - mTarget->_getDerivedPosition()).length();

        if (mOrbiting)   // yaw around the target, and pitch locally
        {
            mCamera->setPosition(mTarget->_getDerivedPosition());

            mCamera->yaw(Ogre::Degree(-evt.state.X.rel * 0.25f));
            mCamera->pitch(Ogre::Degree(-evt.state.Y.rel * 0.25f));

            mCamera->moveRelative(Ogre::Vector3(0, 0, dist));

            // don't let the camera go over the top or around the bottom of the target
        }
        else if (mZooming)  // move the camera toward or away from the target
        {
            // the further the camera is, the faster it moves
            mCamera->moveRelative(Ogre::Vector3(0, 0, evt.state.Y.rel * 0.004f * dist));
        }
        else if (evt.state.Z.rel != 0)  // move the camera toward or away from the target
        {
            // the further the camera is, the faster it moves
            mCamera->moveRelative(Ogre::Vector3(0, 0, -evt.state.Z.rel * 0.0008f * dist));
        }
    }
    else if (mStyle == CS_FREELOOK)
    {
        mCamera->yaw(Ogre::Degree(-evt.state.X.rel * 0.15f));
        mCamera->pitch(Ogre::Degree(-evt.state.Y.rel * 0.15f));
    }
}

/*-----------------------------------------------------------------------------
| Processes mouse presses. Only applies for orbit style.
| Left button is for orbiting, and right button is for zooming.
-----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
void CameraMan::injectMouseDown(const OIS::MultiTouchEvent& evt)
{
    if (mStyle == CS_ORBIT)
    {
        mOrbiting = true;
    }
}
#else
void CameraMan::injectMouseDown(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
    if (mStyle == CS_ORBIT)
    {
        if (id == OIS::MB_Left) mOrbiting = true;
        else if (id == OIS::MB_Right) mZooming = true;
    }
}
#endif

/*-----------------------------------------------------------------------------
| Processes mouse releases. Only applies for orbit style.
| Left button is for orbiting, and right button is for zooming.
-----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
void CameraMan::injectMouseUp(const OIS::MultiTouchEvent& evt)
{
    if (mStyle == CS_ORBIT)
    {
        mOrbiting = false;
    }
}
#else
void CameraMan::injectMouseUp(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
    if (mStyle == CS_ORBIT)
    {
        if (id == OIS::MB_Left) mOrbiting = false;
        else if (id == OIS::MB_Right) mZooming = false;
    }
}
#endif
