#ifndef S3D_ANALYZER_RENDERING_TEXTUREMANAGER_H
#define S3D_ANALYZER_RENDERING_TEXTUREMANAGER_H

class TextureManager {
 public:
  // must be created/deleted with OpenGL context current
  TextureManager();

 private:
  void initTextures();
  void addTexture(const QString& filename);
  void setTextureDefaultProperties(QOpenGLTexture* texture);
  void setImage(const QImage& image, int id);

 public:
  void setImageLeft(const QImage& image);
  void setImageRight(const QImage& image);
  float computeImageAspectRatio(QSize viewportSize);
  QSize getTextureSize();
  QOpenGLTexture* getTexture(int id);
  bool empty();

 private:
  std::vector<std::unique_ptr<QOpenGLTexture>> m_textures;
};

#endif // S3D_ANALYZER_RENDERING_TEXTUREMANAGER_H
