#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "BaseApplication.hpp"
#include "dynamic_assets/VideoPlayer3DEntity.h"

class DynamicTexture;
class DynamicTextureThreadSafe;
class PointCloudMesh;

class Application : public BaseApplication {
 public:
  Application();
  ~Application() override;

 protected:
  bool frameRenderingQueued(const Ogre::FrameEvent& /*evt*/) override;

  void createScene() override;

  void addLights();

  void createGroundPlane();

  static std::unique_ptr<DynamicTextureThreadSafe> createDynamicTexture(const std::string& name);

  void createVideoRectangle(const std::string& materialNameLeft,
                            const std::string& materialNameRight);
  void createVideoPlane(const std::string& materialName);

  void createPointCloud();

  std::unique_ptr<VideoPlayer3DEntity> videoPlayer3DEntity_;

  // to stream image to texture (L, R)
  std::pair<std::unique_ptr<DynamicTextureThreadSafe>, std::unique_ptr<DynamicTextureThreadSafe>>
      m_videoTextures;

  std::pair<std::unique_ptr<Ogre::Rectangle2D>, std::unique_ptr<Ogre::Rectangle2D>> m_Rectangles;

  std::unique_ptr<PointCloudMesh> m_pointCloudMesh;

  // frame request listeners
  std::vector<Ogre::FrameListener*> m_frameListeners;
};

#endif  // APPLICATION_HPP
