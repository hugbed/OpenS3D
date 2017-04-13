#include "Application.hpp"

#include "point_cloud/PointCloudMesh.hpp"
#include "video_texture/DynamicTexture.hpp"
#include "video_texture/DynamicTextureThreadSafe.hpp"
#include "video_texture/TextureUpdateClient.hpp"

#include "s3d/video/capture/file_video_capture_device_3d.h"
#include "dynamic_assets/VideoPlayer3DEntity.h"
#include "dynamic_assets/VideoPlayerEntity.h"

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
  //  createVideoRectangle(m_Rectangles.first, m_videoTextures.first, "L");
  //  createVideoRectangle(m_Rectangles.second, m_videoTextures.second, "R");

  constexpr auto textureNameLeft = "DynamicTextureL";
  constexpr auto textureNameRight = "DynamicTextureR";
  m_videoTextures.first = createDynamicTexture(textureNameLeft);
  m_videoTextures.second = createDynamicTexture(textureNameRight);
  m_frameListeners.push_back(m_videoTextures.first.get());
  m_frameListeners.push_back(m_videoTextures.second.get());

  // default texture creation
  {
    //  constexpr auto materialNameLeft = "DynamicMaterialL";
    //  constexpr auto materialNameRight = "DynamicMaterialR";
    //  DynamicTexture::createImageMaterial(std::string(materialNameLeft), textureNameLeft);
    //  DynamicTexture::createImageMaterial(std::string(materialNameRight), textureNameRight);

    //  material->getTechnique(0)->getPass(1)->createTextureUnitState(textureNameRight);
    //  material->getTechnique(0)->getPass(1)->setDepthCheckEnabled(false);
    //  material->getTechnique(0)->getPass(1)->setDepthWriteEnabled(false);
    //  material->getTechnique(0)->getPass(1)->setLightingEnabled(false);
    //
    //  material->getTechnique(0)->getPass(0)->createTextureUnitState(textureNameLeft);
    //  material->getTechnique(0)->getPass(2)->createTextureUnitState(textureNameRight);
    //  material->getTechnique(0)->getPass(2)->setDepthCheckEnabled(false);
    //  material->getTechnique(0)->getPass(2)->setDepthWriteEnabled(false);
    //  material->getTechnique(0)->getPass(2)->setLightingEnabled(false);
  }

  // anaglyph
  if (false) {
    // anaglyph material
    auto material = static_cast<Ogre::MaterialPtr>(
        Ogre::MaterialManager::getSingleton().getByName("Anaglyph", "General"));
    auto textureUnit =
        material->getTechnique(0)->getPass(0)->createTextureUnitState(textureNameLeft);
    textureUnit->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
    textureUnit->setTextureBorderColour(Ogre::ColourValue::Black);
    textureUnit = material->getTechnique(0)->getPass(0)->createTextureUnitState(textureNameRight);
    textureUnit->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
    textureUnit->setTextureBorderColour(Ogre::ColourValue::Black);
    material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
    material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
    material->getTechnique(0)->getPass(0)->setLightingEnabled(false);

    // set horizontal shift (%)
    material->getTechnique(0)->getPass(0)->getVertexProgramParameters()->setNamedConstant(
        "horizontalShift", Ogre::Real(0.02));

    // todo: delete that
    VideoPlayerEntity* entity = new VideoPlayerEntity("entititi", "Anaglyph");
    mSceneMgr->getRootSceneNode()->createChildSceneNode("entititiNode")->attachObject(entity);
  }

  // side by side
  {
    auto materialLeft = static_cast<Ogre::MaterialPtr>(
        Ogre::MaterialManager::getSingleton().getByName("StereoSide", "General"));
    materialLeft->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
    materialLeft->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
    materialLeft->getTechnique(0)->getPass(0)->setLightingEnabled(false);

    auto textureUnitLeft =
        materialLeft->getTechnique(0)->getPass(0)->createTextureUnitState(textureNameLeft);
    textureUnitLeft->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
    textureUnitLeft->setTextureBorderColour(Ogre::ColourValue::Black);

    auto materialRight = materialLeft->clone("StereoSideRight");

    auto textureUnitRight =
        materialRight->getTechnique(0)->getPass(0)->createTextureUnitState(textureNameLeft);
    textureUnitRight->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
    textureUnitRight->setTextureBorderColour(Ogre::ColourValue::Black);

    constexpr Ogre::Real horizontalShift = 0.015;

    materialLeft->getTechnique(0)->getPass(0)->getVertexProgramParameters()->setNamedConstant(
        "horizontalShift", horizontalShift);
    materialRight->getTechnique(0)->getPass(0)->getVertexProgramParameters()->setNamedConstant(
        "horizontalShift", -horizontalShift);

    // todo: delete that
    VideoPlayer3DEntity* entity =
        new VideoPlayer3DEntity("entititi", "StereoSide", "StereoSideRight");
    mSceneMgr->getRootSceneNode()->createChildSceneNode("entititiNode")->attachObject(entity);
  }

  //  createVideoRectangle(materialNameLeft, materialNameRight);
  //  createVideoPlane("PointCloud");

  // should delete this pointer, this is only a test
  auto captureDevice = new FileVideoCaptureDevice3D(
      "/home/jon/Videos/current-left.yuv;/home/jon/Videos/current-right.yuv");

  auto captureClient =
      std::unique_ptr<VideoCaptureDevice3D::Client>(std::make_unique<TextureUpdateClient>(
          m_videoTextures.first.get(), m_videoTextures.second.get()));

  VideoCaptureFormat format;  // todo(hugbed): could be used to init texture size
  captureDevice->AllocateAndStart(format, std::move(captureClient));

  //  addLights();
  //  createGroundPlane();
  //  createPointCloud();
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
      "ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1500, 1500, 20, 20,
      true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
  Ogre::Entity* groundEntity = mSceneMgr->createEntity("ground");
  mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
  groundEntity->setCastShadows(false);
  groundEntity->setMaterialName("Rockwall");

  groundEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);
}

std::unique_ptr<DynamicTextureThreadSafe> Application::createDynamicTexture(
    const std::string& name) {
  // twice as fast as update to prevent aliasing (if this makes sense)
  constexpr auto refreshRate = 1.0f / 60.0f;
  return std::make_unique<DynamicTextureThreadSafe>(name, Ogre::PixelFormat::PF_R8G8B8, 1920, 1080,
                                                    refreshRate);
}

void Application::createVideoRectangle(const std::string& materialNameLeft,
                                       const std::string& materialNameRight) {
  constexpr auto videoPlayerName = "VideoPlayerEntity";
  videoPlayer3DEntity_ =
      std::make_unique<VideoPlayer3DEntity>(videoPlayerName, materialNameLeft, materialNameRight);
  mSceneMgr->getRootSceneNode()
      ->createChildSceneNode(videoPlayerName)
      ->attachObject(videoPlayer3DEntity_.get());
}

void Application::createVideoPlane(const std::string& materialName) {
  constexpr auto videoPlaneStr = "VideoPlane";

  // instead let's create a cube
  Ogre::Plane plane(Ogre::Vector3::UNIT_Z, 0);
  Ogre::MeshManager::getSingleton().createPlane(
      videoPlaneStr, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1.6f,
      0.9f,  // w, h
      1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Y);

  Ogre::Entity* groundEntity = mSceneMgr->createEntity(videoPlaneStr);
  groundEntity->setMaterialName(materialName);

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
      "greatpointcloud", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, points, colors);

  Ogre::Entity* pcEnt = mSceneMgr->createEntity("pointcloudentity", "yobitch");
  pcEnt->setMaterialName("PointCloud");

  // todo(hugbed): weird that I have to set this to background
  //  pcEnt->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAIN);

  auto sceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
  sceneNode->setPosition(0.0f, 1.5f, 0.2f);
  sceneNode->attachObject(pcEnt);
}