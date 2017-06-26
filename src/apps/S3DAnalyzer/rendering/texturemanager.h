#ifndef RENDERING_TEXTUREMANAGER_H
#define RENDERING_TEXTUREMANAGER_H

#include <QSize>
#include <QImage>

#include <memory>

class QOpenGLTexture;
class QString;

class TextureManager {
 public:
  // must be created/deleted with OpenGL context current
  TextureManager();
  ~TextureManager();

 private:
  void initTextures();
  void addTexture(const QString& filename);
  void setTextureDefaultProperties(QOpenGLTexture* texture);
  void fromImage(const QImage& image, int id);

 public:
  // updates dirty textures (from video) in main thread
  void update();

  void setImageLeft(const QImage& image);
  void setImageRight(const QImage& image);
  const QImage& getImageLeft();
  const QImage& getImageRight();
  float computeImageAspectRatio(QSize viewportSize);
  QSize getTextureSize();
  QOpenGLTexture* getTexture(int id);
  bool empty();

 private:
  std::vector<QImage> m_images;
  std::vector<std::unique_ptr<QOpenGLTexture>> m_textures;
  std::vector<bool> m_dirty;
};

#endif  // RENDERING_TEXTUREMANAGER_H
