#ifndef RENDERING_TEXTUREMANAGER_H
#define RENDERING_TEXTUREMANAGER_H

#include <QSize>
#include <QImage>

#include <memory>
#include <mutex>

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
  // updates dirty textures (transfer images to OpenGL textures)
  void update();

  void setImageLeft(const QImage& image);
  void setImageRight(const QImage& image);
  void setImages(const QImage& imageLeft, const QImage& imageRight);
  bool imagesDirty();
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
