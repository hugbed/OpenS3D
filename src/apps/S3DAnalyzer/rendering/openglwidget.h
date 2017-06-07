#ifndef S3D_ANALYZER_RENDERING_OPENGLWIDGET_H
#define S3D_ANALYZER_RENDERING_OPENGLWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QImage>

#include "rendering/entity/billboard/billboardintensityentity.h"
#include "entity/stereo/rectangleentity.h"
#include "entity/stereo/stereoimageentity.h"

#include <memory>
#include <vector>

class QOpenGLShaderProgram;

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
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

  ~OpenGLWidget() override;
  explicit OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {}

  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

  void setImageLeft(const QImage& image);
  void setImageRight(const QImage& image);
  void setHorizontalShift(float shift);
  void toggleFeatures(bool display = false);

  void setFeatures(std::vector<QVector2D> points, std::vector<float> disparities);

 public slots:
  void displayModeChanged(DisplayMode mode);

 private:
  void initTextures();
  void addTexture(const QString& filename);
  void setTextureDefaultProperties(QOpenGLTexture* texture);
  void initEntities();
  void drawEntities();
  float computeImageAspectRatio();

  template <class T>
  void createEntity(DisplayMode mode);

  QSize m_viewportSize;
  float m_horizontalShift;

  std::vector<std::unique_ptr<QOpenGLTexture>> m_textures;
  DisplayMode m_currentMode;

  // todo: such arrays may lead to out of bounds
  std::unique_ptr<StereoImageEntity>
      m_stereoEntities[static_cast<int>(DisplayMode::NB_DISPLAY_MODES)];

  bool m_showOverlay{false};
  std::unique_ptr<BillboardIntensityEntity> m_overlay;
};

#endif  // S3D_ANALYZER_RENDERING_OPENGLWIDGET_H
