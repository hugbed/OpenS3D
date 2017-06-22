#include "texturemanager.h"

#include <QColor>
#include <QImage>
#include <QOpenGLTexture>
#include <QString>

// must be created with OpenGL context current
TextureManager::TextureManager()
{
  initTextures();
}

void TextureManager::initTextures() {
  addTexture(":/images/left.jpg");
  addTexture(":/images/right.jpg");
}

void TextureManager::addTexture(const QString& filename) {
  auto texture = std::make_unique<QOpenGLTexture>((QImage(filename).mirrored()));
  setTextureDefaultProperties(texture.get());
  m_textures.push_back(std::move(texture));
}

void TextureManager::setTextureDefaultProperties(QOpenGLTexture* texture) {
  texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
  texture->setMagnificationFilter(QOpenGLTexture::Linear);
  texture->setWrapMode(QOpenGLTexture::ClampToBorder);
  texture->setBorderColor(Qt::black);
}

void TextureManager::setImage(const QImage& image, int id)
{
  auto&& texture = m_textures[0];
  texture->destroy();
  texture->create();
  texture->setData(image.mirrored(false, true));
  setTextureDefaultProperties(texture.get());
}

void TextureManager::setImageLeft(const QImage& image) {
  setImage(image, 0);
}

void TextureManager::setImageRight(const QImage& image) {
  setImage(image, 1);
}

float TextureManager::computeImageAspectRatio(QSize viewportSize) {
  if (m_textures.empty() || m_textures[0] == nullptr) {
    return 1.0f;
  }

  float vW = viewportSize.width(),  //
      vH = viewportSize.height(),   //
      iW = m_textures[0]->width(),  //
      iH = m_textures[0]->height();

  return (vW / vH) / (iW / iH);
}

QSize TextureManager::getTextureSize()
{
  if (m_textures.empty() || m_textures[0] == nullptr) {
    return {};
  }
  return QSize(m_textures[0]->width(), m_textures[0]->height());
}

QOpenGLTexture* TextureManager::getTexture(int id) {
  if (id >= m_textures.size()) {
    return nullptr;
  }
  return m_textures[id].get();
}

bool TextureManager::empty()
{
  return m_textures.empty() || m_textures[0] == nullptr;
}
