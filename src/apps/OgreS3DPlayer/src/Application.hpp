#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "BaseApplication.hpp"

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

  void createVideoRectangle(
      std::unique_ptr<Ogre::Rectangle2D>& rect,
      std::unique_ptr<DynamicTextureThreadSafe>& videoTexture,
      const std::string& id);

  void createVideoPlane(std::unique_ptr<DynamicTextureThreadSafe>& videoTexture,
                        const std::string& id);

  void createPointCloud();

  // to stream image to texture (L, R)
  std::pair<std::unique_ptr<DynamicTextureThreadSafe>,
            std::unique_ptr<DynamicTextureThreadSafe>>
      m_videoTextures;

  std::pair<std::unique_ptr<Ogre::Rectangle2D>,
            std::unique_ptr<Ogre::Rectangle2D>>
      m_Rectangles;

  std::unique_ptr<PointCloudMesh> m_pointCloudMesh;

  // frame request listeners
  std::vector<Ogre::FrameListener*> m_frameListeners;
};

#endif  // APPLICATION_HPP
