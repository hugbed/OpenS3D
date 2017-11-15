#include "texturemanager.h"

#include <QColor>
#include <QImage>
#include <QOpenGLTexture>

#include <cassert>

// must be created with OpenGL context current
TextureManager::TextureManager() {
  initTextures();
}

TextureManager::~TextureManager() = default;

void TextureManager::initTextures() {
  addTexture(":/images/left.jpg");
  addTexture(":/images/right.jpg");
  m_images.emplace_back(":/images/left.jpg");
  m_images.emplace_back(":/images/right.jpg");
  m_dirty.resize(2, false);
}

void TextureManager::addTexture(const QString& filename) {
  auto texture = std::make_unique<QOpenGLTexture>((QImage(filename).mirrored()));
  setTextureDefaultProperties(texture.get());
  m_textures.push_back(std::move(texture));
  m_dirty.push_back(false);
}

void TextureManager::setTextureDefaultProperties(QOpenGLTexture* texture) {
  texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
  texture->setMagnificationFilter(QOpenGLTexture::Linear);
  texture->setWrapMode(QOpenGLTexture::ClampToBorder);
  texture->setBorderColor(Qt::black);
}

void TextureManager::fromImage(const QImage& image, size_t id) {
  if (id >= m_textures.size()) {
    return;
  }

  auto&& texture = m_textures[id];
  texture->destroy();
  texture->create();
  texture->setData(image.mirrored(false, true));
  setTextureDefaultProperties(texture.get());

  m_dirty[id] = false;
}

void TextureManager::setImageLeft(const QImage& image) {
  constexpr int leftIdx = 0;
  m_images[leftIdx] = image;
  m_dirty[leftIdx] = true;
}

void TextureManager::setImageRight(const QImage& image) {
  constexpr int rightIdx = 1;
  m_images[rightIdx] = image;
  m_dirty[rightIdx] = true;
}

void TextureManager::setImages(const QImage& imageLeft, const QImage& imageRight) {
  setImageLeft(imageLeft);
  setImageRight(imageRight);
}

const QImage& TextureManager::getImageLeft() {
  return m_images[0];
}

const QImage& TextureManager::getImageRight() {
  return m_images[1];
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

QSize TextureManager::getTextureSize() {
  if (m_textures.empty() || m_textures[0] == nullptr) {
    return {};
  }
  return QSize(m_textures[0]->width(), m_textures[0]->height());
}

QOpenGLTexture* TextureManager::getTexture(size_t id) {
  if (id >= m_textures.size()) {
    return nullptr;
  }
  return m_textures[id].get();
}

bool TextureManager::empty() {
  return m_textures.empty() || m_textures[0] == nullptr;
}

void TextureManager::update() {
  assert(m_textures.size() == m_dirty.size() && m_textures.size() == m_images.size());

  for (auto i = 0UL; i < m_textures.size(); ++i) {
    if (m_dirty[i]) {
      fromImage(m_images[i], i);
    }
  }
}

bool TextureManager::imagesDirty() {
  return std::all_of(std::begin(m_dirty), std::end(m_dirty), [](bool val) { return val; });
}
