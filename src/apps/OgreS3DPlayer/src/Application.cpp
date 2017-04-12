#include "Application.hpp"

#include "point_cloud/PointCloudMesh.hpp"
#include "video_texture/DynamicTexture.hpp"
#include "video_texture/DynamicTextureThreadSafe.hpp"
#include "video_texture/TextureUpdateClient.hpp"

#include "s3d/video/capture/file_video_capture_device_3d.h"

#include <OgreRectangle2D.h>

//-------------------------------------------------------------------------------------

Application::Application() = default;
Application::~Application() = default;

//-------------------------------------------------------------------------------------

bool Application::frameRenderingQueued(const Ogre::FrameEvent& evt) {
  for (auto& listeners : m_frameListeners) {
    listeners->frameRenderingQueued(evt);
  }
  return BaseApplication::frameRenderingQueued(evt);
}

//-------------------------------------------------------------------------------------
void Application::createScene() {
  //    createVideoRectangle(m_Rectangles.first, m_videoTextures.first, "L");
  //    createVideoRectangle(m_Rectangles.second, m_videoTextures.second, "R");

  createVideoPlane(m_videoTextures.first, "_plane");
  createVideoPlane(m_videoTextures.second, "_plane2");

  // should delete this pointer, this is only a test
  auto captureDevice = new FileVideoCaptureDevice3D(
      "/home/jon/Videos/current-left.yuv;/home/jon/Videos/current-right.yuv");

  auto captureClient = std::unique_ptr<VideoCaptureDevice3D::Client>(
      std::make_unique<TextureUpdateClient>(m_videoTextures.first.get(),
                                            m_videoTextures.second.get()));

  VideoCaptureFormat format;
  captureDevice->AllocateAndStart(format, std::move(captureClient));

  addLights();
  createGroundPlane();
  createPointCloud();
}

void Application::addLights() {
  Ogre::Light* directionalLight = mSceneMgr->createLight("DirectionalLight");
  directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
  directionalLight->setDiffuseColour(Ogre::ColourValue(.4, .4, .4));
  directionalLight->setSpecularColour(Ogre::ColourValue(.4, .4, .4));
  directionalLight->setDirection(Ogre::Vector3(0, -1, 1));
  Ogre::Light* pointLight = mSceneMgr->createLight("PointLight");
  pointLight->setType(Ogre::Light::LT_POINT);
  pointLight->setDiffuseColour(.3, .3, .3);
  pointLight->setSpecularColour(.3, .3, .3);
  pointLight->setPosition(Ogre::Vector3(0, 150, 250));
}

void Application::createGroundPlane() {
  Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
  Ogre::MeshManager::getSingleton().createPlane(
      "ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
      1500, 1500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
  Ogre::Entity* groundEntity = mSceneMgr->createEntity("ground");
  mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(
      groundEntity);
  groundEntity->setCastShadows(false);
  groundEntity->setMaterialName("Rockwall");
}

void Application::createVideoRectangle(
    std::unique_ptr<Ogre::Rectangle2D>& rect,
    std::unique_ptr<DynamicTextureThreadSafe>& videoTexture,
    const std::string& id) {
  constexpr auto refreshRate = 1.0f / 60.0f;  // twice as fast as update to
  // prevent aliasing (if this makes
  // sense)

  // Using: http://ogre3d.org/tikiwiki/Creating+dynamic+textures
  // todo: should probably create video textures outside to assign them to
  // multiple places without recreating them
  videoTexture = std::make_unique<DynamicTextureThreadSafe>(
      std::string("DynamicTexture").append(id), Ogre::PixelFormat::PF_R8G8B8,
      1920, 1080, refreshRate);
  auto material = DynamicTexture::createImageMaterial(
      std::string("BackgroundMaterial").append(id),
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
  mSceneMgr->getRootSceneNode()
      ->createChildSceneNode(std::string("Background").append(id))
      ->attachObject(rect.get());
}

void Application::createVideoPlane(
    std::unique_ptr<DynamicTextureThreadSafe>& videoTexture,
    const std::string& id) {
  constexpr auto refreshRate = 1.0f / 60.0f;  // twice as fast as update to
  // prevent aliasing (if this makes
  // sense)

  // Using: http://ogre3d.org/tikiwiki/Creating+dynamic+textures
  videoTexture = std::make_unique<DynamicTextureThreadSafe>(
      std::string("DynamicTexture").append(id), Ogre::PixelFormat::PF_R8G8B8,
      1920, 1080, refreshRate);
  auto material = Ogre::MaterialManager::getSingleton().create(
      std::string("BackgroundMaterial").append(id), "General");
  material->getTechnique(0)->getPass(0)->createTextureUnitState(
      std::string("DynamicTexture").append(id));
  material->getTechnique(0)->getPass(0)->setLightingEnabled(false);

  m_frameListeners.push_back(videoTexture.get());

  // instead let's create a cube
  Ogre::Plane plane(Ogre::Vector3::UNIT_Z, 0);
  Ogre::MeshManager::getSingleton().createPlane(
      std::string("ground").append(id),
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1.6f,
      0.9f,  // w, h
      1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Y);

  Ogre::Entity* groundEntity =
      mSceneMgr->createEntity(std::string("ground").append(id));
  groundEntity->setMaterial(material);

  auto sceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
  sceneNode->setPosition(0.0f, 1.5f, 0.0f);
  sceneNode->attachObject(groundEntity);
}

void Application::createPointCloud() {
  // try to create a 16x9 point cloud with 1920x1080 resolution
  // naive version, just to test the point cloud
  constexpr auto rectangleW = 1.6f, rectangleH = 0.9f;
  constexpr auto imageWidth = 1920, imageHeight = 1080;
  constexpr auto worldPixelHeight = rectangleH / imageHeight;
  constexpr auto worldPixelWidth = rectangleW / imageWidth;
  constexpr auto jOffset = -imageWidth / 2.0f * worldPixelWidth;
  constexpr auto iOffset = -imageHeight / 2.0f * worldPixelHeight;
  constexpr auto samplingRatio = 10;

  constexpr auto nbVertices = imageHeight * imageWidth / samplingRatio;
  std::vector<float> points(static_cast<size_t>(nbVertices)),
      colors(static_cast<size_t>(nbVertices));
  for (auto i = 0; i < imageHeight; i += samplingRatio) {
    auto posY = 0.5f * worldPixelHeight + i * worldPixelHeight + iOffset;
    for (auto j = 0; j < imageWidth; j += samplingRatio) {
      auto posX = 0.5f * worldPixelWidth + j * worldPixelWidth + jOffset;
      points.emplace_back(posX);
      points.emplace_back(posY);
      points.emplace_back(0.0f);
      colors.emplace_back(posX);  // r
      colors.emplace_back(posY);  // g
      colors.emplace_back(0.0f);  // b
    }
  }

  m_pointCloudMesh = std::make_unique<PointCloudMesh>(
      "greatpointcloud",
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, points, colors);

  Ogre::Entity* pcEnt = mSceneMgr->createEntity("pointcloudentity", "yobitch");
  pcEnt->setMaterialName("PointCloud");

  auto sceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
  sceneNode->setPosition(0.0f, 1.5f, 0.2f);
  sceneNode->attachObject(pcEnt);
}