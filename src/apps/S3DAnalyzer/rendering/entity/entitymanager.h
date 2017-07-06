#ifndef RENDERING_ENTITY_ENTITYMANAGER_H
#define RENDERING_ENTITY_ENTITYMANAGER_H

#include <QSize>
#include <QVector2D>

#include <memory>

class TextureManager;
class StereoImageEntity;
class BillboardIntensityEntity;

class EntityManager {
 public:
  enum class DisplayMode {
    Anaglyph = 0,
    Opacity = 1,
    Interlaced = 2,
    SideBySide = 3,
    Left = 4,
    Right = 5,
    NB_DISPLAY_MODES = 6
  };

  // must be created with OpenGL context current
  explicit EntityManager(TextureManager* textureManager);

  ~EntityManager();

  template <class T>
  void createEntity(DisplayMode mode);

  void drawEntities(QSize viewportSize);
  void setFeaturesVisibility(bool display);
  void setFeatures(std::vector<QVector2D> points, std::vector<float> disparities);
  void setHorizontalShift(float horizontalShift);
  float getHorizontalShift() const;

 public slots:
  void displayModeChanged(EntityManager::DisplayMode mode);

 private:
  DisplayMode m_currentMode;
  float m_horizontalShift{0};
  bool m_showOverlay{false};
  std::unique_ptr<StereoImageEntity>
      m_stereoEntities[static_cast<int>(DisplayMode::NB_DISPLAY_MODES)];
  std::unique_ptr<BillboardIntensityEntity> m_overlay;
  TextureManager* m_textureManager;
};

#endif  // RENDERING_ENTITY_ENTITYMANAGER_H
