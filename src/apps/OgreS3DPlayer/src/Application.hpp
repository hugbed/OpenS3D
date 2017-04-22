#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "BaseApplication.hpp"

class DynamicTexture;
class DynamicTextureThreadSafe;
class PointCloudMesh;
class VideoCaptureDevice;
class VideoCaptureFormat;

class Application : public BaseApplication {
 public:
  Application();
  ~Application() override;

 protected:
  bool frameRenderingQueued(const Ogre::FrameEvent& /*evt*/) override;

  void createScene() override;

  void addLights();

  void createGroundPlane();

  static std::unique_ptr<DynamicTextureThreadSafe> createDynamicTexture(
      const std::string& name,
      const VideoCaptureFormat& format);

  void createVideoPlane(const std::string& materialName);

  void createPointCloud();

  std::unique_ptr<Ogre::MovableObject> videoPlayerEntity_;
  std::unique_ptr<VideoCaptureDevice> videoCaptureDevice_;

  // to stream image to texture (L, R)
  std::pair<std::unique_ptr<DynamicTextureThreadSafe>, std::unique_ptr<DynamicTextureThreadSafe>>
      m_videoTextures;

  std::unique_ptr<PointCloudMesh> m_pointCloudMesh;

  // frame request listeners
  std::vector<Ogre::FrameListener*> m_frameListeners;
};

#endif  // APPLICATION_HPP
