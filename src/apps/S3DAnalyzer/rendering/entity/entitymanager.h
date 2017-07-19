#ifndef RENDERING_ENTITY_ENTITYMANAGER_H
#define RENDERING_ENTITY_ENTITYMANAGER_H

#include <QSize>
#include <QVector2D>

#include <Eigen/Dense>

#include <memory>

class TextureManager;
class StereoImageEntity;
class BillboardDisparityEntity;
class BillboardDepthEntity;
class QPaintDevice;
class UserSettings;
class ViewerCentricEntity;

class EntityManager {
 public:
  enum class DisplayMode {
    Anaglyph,
    Opacity,
    Interlaced,
    SideBySide,
    Left,
    Right,
    ViewerCentric,
    NB_DISPLAY_MODES
  };

  // must be created with OpenGL context current
  explicit EntityManager(TextureManager* textureManager);

  ~EntityManager();

  void drawEntities(QPaintDevice* paintDevice);
  void setFeaturesVisibility(bool display);
  void setFeatures(std::vector<Eigen::Vector2f> points, std::vector<float> disparities);
  void setHorizontalShift(float horizontalShift);
  float getHorizontalShift() const;
  void setUserSettings(UserSettings* userSettings);

 public slots:
  void displayModeChanged(EntityManager::DisplayMode mode);

 private:
  template <class T>
  void createEntity(DisplayMode mode);

  template <class T>
  void createEntity(DisplayMode mode, std::unique_ptr<T> t);

  void adjustDepthRanges();
  void drawCurrentEntity(QPaintDevice* paintDevice, float aspectRatio);
  void drawCurrentBillboard(QPaintDevice* paintDevice, float aspectRatio);

  UserSettings* m_userSettings{};
  DisplayMode m_currentMode;
  float m_horizontalShift{0};
  bool m_showOverlay{false};
  std::unique_ptr<StereoImageEntity>
      m_stereoEntities[static_cast<int>(DisplayMode::NB_DISPLAY_MODES)];
  std::unique_ptr<BillboardDisparityEntity> m_billboardImage;
  std::unique_ptr<BillboardDepthEntity> m_billboardWorld;

  ViewerCentricEntity* m_viewerCentricEntity{};
  BillboardDisparityEntity* m_currentBillboard{};

  TextureManager* m_textureManager;
};

#endif  // RENDERING_ENTITY_ENTITYMANAGER_H
