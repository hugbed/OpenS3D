#include "stereosidebysideentity.h"

StereoSideBySideEntity::StereoSideBySideEntity() = default;

void StereoSideBySideEntity::init() {
  m_rectangles[0].init(-1.0f, 1.0f, 0.0, -1.0f);
  m_rectangles[1].init(0.0f, 1.0f, 1.0, -1.0f);
}

void StereoSideBySideEntity::setTextureLeft(QOpenGLTexture* texture) {
  m_rectangles[0].setTexture(0, texture);
}

void StereoSideBySideEntity::setTextureRight(QOpenGLTexture* texture) {
  m_rectangles[1].setTexture(1, texture);
}

void StereoSideBySideEntity::draw(QPaintDevice* parent) {
  for (auto& rectangle : m_rectangles) {
    rectangle.draw(parent);
  }
}

void StereoSideBySideEntity::setHorizontalShift(float shift) {
  m_rectangles[0].setHorizontalShift(shift / 2.0f);
  m_rectangles[1].setHorizontalShift(-shift / 2.0f);
}

void StereoSideBySideEntity::setAspectRatio(float ratio) {
  for (auto& rectangle : m_rectangles) {
    rectangle.setAspectRatio(ratio / 2.0f);
  }
}
