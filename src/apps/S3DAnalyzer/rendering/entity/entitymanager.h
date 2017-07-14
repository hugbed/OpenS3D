#ifndef RENDERING_ENTITY_ENTITYMANAGER_H
#define RENDERING_ENTITY_ENTITYMANAGER_H

#include <QSize>
#include <QVector2D>

#include <memory>

class TextureManager;
class StereoImageEntity;
class BillboardIntensityEntity;
class BillboardIntensityWorldEntity;
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

  void drawEntities(QPaintDevice* parent, QSize viewportSize);
  void setFeaturesVisibility(bool display);
  void setFeatures(std::vector<QVector2D> points, std::vector<float> disparities);
  void setHorizontalShift(float horizontalShift);
  float getHorizontalShift() const;
  void setUserSettings(UserSettings* displayParameters);

 public slots:
  void displayModeChanged(EntityManager::DisplayMode mode);

 private:
  template <class T>
  void createEntity(DisplayMode mode);

  template <class T>
  void createEntity(DisplayMode mode, std::unique_ptr<T> t);

  UserSettings* m_userSettings{};
  DisplayMode m_currentMode;
  float m_horizontalShift{0};
  bool m_showOverlay{false};
  std::unique_ptr<StereoImageEntity>
      m_stereoEntities[static_cast<int>(DisplayMode::NB_DISPLAY_MODES)];
  std::unique_ptr<BillboardIntensityEntity> m_billboardImage;
  std::unique_ptr<BillboardIntensityWorldEntity> m_billboardWorld;

  ViewerCentricEntity* m_viewerCentricEntity{};
  BillboardIntensityEntity* m_currentBillboard{};

  TextureManager* m_textureManager;
};

#endif  // RENDERING_ENTITY_ENTITYMANAGER_H
