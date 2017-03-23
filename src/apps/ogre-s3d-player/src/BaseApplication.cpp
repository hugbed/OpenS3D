#include "BaseApplication.hpp"

//#include <OgreRoot.h>
//#include <OgreRenderWindow.h>
//#include <OIS/OIS.h>

//-------------------------------------------------------------------------------------
BaseApplication::BaseApplication()
    : mFSLayer(OGRE_NEW_T(Ogre::FileSystemLayer, Ogre::MEMCATEGORY_GENERAL)(OGRE_VERSION_NAME),
        [](Ogre::FileSystemLayer* fsl){
            OGRE_DELETE_T(fsl, FileSystemLayer, Ogre::MEMCATEGORY_GENERAL);
        })
    , mRoot{}
    , mCamera(nullptr)
    , mSceneMgr(nullptr)
    , mWindow(nullptr)
    , mResourcesCfg(Ogre::BLANKSTRING)
    , mPluginsCfg(Ogre::BLANKSTRING)
	, mPolygonRenderingMode('B')
    , mShutDown(false)
    , mInputManager(nullptr)
    , mMouse(nullptr)
    , mKeyboard(nullptr)
    , mCameraMan{}
{
}

//-------------------------------------------------------------------------------------
BaseApplication::~BaseApplication()
{
	if (mRoot) { mRoot->removeFrameListener(this); }

    //Remove ourself as a Window listener
	if (mWindow) {
    	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    	windowClosed(mWindow);
	}
}

//-------------------------------------------------------------------------------------
bool BaseApplication::configure()
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    if(mRoot->showConfigDialog(nullptr))
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise(true, "Ogre-Sample Application Render Window");

        return true;
    }
    else
    {
        return false;
    }
}
//-------------------------------------------------------------------------------------
void BaseApplication::chooseSceneManager()
{
    // Get the SceneManager, in this case a generic one
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
}
//-------------------------------------------------------------------------------------
void BaseApplication::createCamera()
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");

    // Position it at 500 in Z direction
    mCamera->setPosition(Ogre::Vector3(0,0,8));
    // Look back along -Z
    mCamera->lookAt(Ogre::Vector3(0,0,-300));
    mCamera->setNearClipDistance(5);

    mCameraMan = std::make_unique<OgreCookies::CameraMan>(mCamera);   // create a default camera controller
    mCameraMan->setTopSpeed(2.0f);
}
//-------------------------------------------------------------------------------------
void BaseApplication::createFrameListener()
{
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem( pl );

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

    //Set initial mouse clipping size
    windowResized(mWindow);

    //Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

    mRoot->addFrameListener(this);
}
//-------------------------------------------------------------------------------------
void BaseApplication::destroyScene()
{
}
//-------------------------------------------------------------------------------------
void BaseApplication::createViewports()
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
//    mWindow->setFullscreen(true, vp->getActualWidth(), vp->getActualHeight());
}
//-------------------------------------------------------------------------------------
void BaseApplication::setupResources()
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(mResourcesCfg);

    // Go through all sections & settings in the file
    auto settingsSec = cf.getSettingsBySection();

    Ogre::String secName, typeName, archName;

    for (auto sec_it = std::begin(settingsSec); sec_it != std::end(settingsSec); ++sec_it)
    {
        secName = (*sec_it).first;
        auto settings = (*sec_it).second;
        for (auto config_it = std::begin(settings); config_it != std::end(settings); ++config_it)
        {
            typeName = config_it->first;
            archName = config_it->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}
//-------------------------------------------------------------------------------------
void BaseApplication::createResourceListener()
{

}
//-------------------------------------------------------------------------------------
void BaseApplication::loadResources()
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//-------------------------------------------------------------------------------------
void BaseApplication::run()
{
#ifdef _DEBUG
    mResourcesCfg = "resources_d.cfg";
    mPluginsCfg = "plugins_d.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif

    if (!setup())
        return;

//    while(true)
//    {
//        Ogre::WindowEventUtilities::messagePump();
//        if(mWindow->isClosed()) break;
//        if(!mRoot->renderOneFrame()) break;
//    }

    mRoot->startRendering();

    // clean up
    destroyScene();
}
//-------------------------------------------------------------------------------------
bool BaseApplication::setup()
{
    mRoot = std::make_unique<Ogre::Root>(mPluginsCfg, mFSLayer->getWritablePath("ogre.cfg"),
				mFSLayer->getWritablePath("ogre.log"));

    setupResources();

    bool carryOn = configure();
    if (!carryOn) return false;

    chooseSceneManager();
    createCamera();
    createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Create any resource listeners (for loading screens)
    createResourceListener();
    // Load resources
    loadResources();

    // Create the scene
    createScene();

    createFrameListener();

    return true;
};
//-------------------------------------------------------------------------------------
bool BaseApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if (mWindow->isClosed()) {
		return false;
	}

	if (mShutDown) {
		return false;
	}

    //Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();

    // could have a list of frame listeners
    mCameraMan->frameRenderingQueued(evt);   // if dialog isn't up, then update the camera

    return true;
}
//-------------------------------------------------------------------------------------
bool BaseApplication::keyPressed( const OIS::KeyEvent &arg )
{
    if (arg.key == OIS::KC_T)   // cycle polygon rendering mode
    {
        Ogre::TextureFilterOptions tfo;
        unsigned int aniso;

        switch (mPolygonRenderingMode)
        {
        case 'B':
			mPolygonRenderingMode = 'T';
            tfo = Ogre::TFO_TRILINEAR;
            aniso = 1;
            break;
        case 'T':
			mPolygonRenderingMode = 'A';
            tfo = Ogre::TFO_ANISOTROPIC;
            aniso = 8;
            break;
        case 'A':
			mPolygonRenderingMode = 'N';
            tfo = Ogre::TFO_NONE;
            aniso = 1;
            break;
        default:
			mPolygonRenderingMode = 'B';
            tfo = Ogre::TFO_BILINEAR;
            aniso = 1;
        }

        Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
        Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
    }
    else if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
    {
        Ogre::PolygonMode pm;

        switch (mCamera->getPolygonMode())
        {
        case Ogre::PM_SOLID:
            pm = Ogre::PM_WIREFRAME;
            break;
        case Ogre::PM_WIREFRAME:
            pm = Ogre::PM_POINTS;
            break;
        default:
            pm = Ogre::PM_SOLID;
        }

        mCamera->setPolygonMode(pm);
    }
    else if(arg.key == OIS::KC_F5)   // refresh all textures
    {
        Ogre::TextureManager::getSingleton().reloadAll();
    }
    else if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
    {
        mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
    }
    else if (arg.key == OIS::KC_ESCAPE)
    {
        mShutDown = true;
    }
    else
    {
        mCameraMan->injectKeyDown(arg);
    }

    return true;
}

bool BaseApplication::keyReleased( const OIS::KeyEvent &arg )
{
    mCameraMan->injectKeyUp(arg);
    return true;
}

bool BaseApplication::mouseMoved( const OIS::MouseEvent &arg )
{
    mCameraMan->injectMouseMove(arg);
    return true;
}

bool BaseApplication::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    mCameraMan->injectMouseDown(arg, id);
    return true;
}

bool BaseApplication::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    mCameraMan->injectMouseUp(arg, id);
    return true;
}

//Adjust mouse clipping area
void BaseApplication::windowResized(Ogre::RenderWindow *rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void BaseApplication::windowClosed(Ogre::RenderWindow *rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow )
    {
        if( mInputManager )
        {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );

            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = nullptr;
        }
    }
}
