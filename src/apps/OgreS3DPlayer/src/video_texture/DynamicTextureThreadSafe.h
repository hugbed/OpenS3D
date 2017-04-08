//
// Created by jon on 18/03/17.
//

#ifndef OGRE_SAMPLE_VIDEOTEXTURE_H
#define OGRE_SAMPLE_VIDEOTEXTURE_H

#include "DynamicTexture.h"

#include <atomic>
#include <chrono>
#include <mutex>

// DynamicTexture with frame rate checks and thread safe texture update
class DynamicTextureThreadSafe : public DynamicTexture,
                                 public Ogre::FrameListener {
 public:
  DynamicTextureThreadSafe(const std::string& textureName,
                           Ogre::PixelFormat format,
                           ushort imgWidth,
                           ushort imgHeight,
                           float timePerFrame)
      : DynamicTexture(textureName, format, imgWidth, imgHeight),
        m_frameMutex{},
        m_nextFrame{},
        m_frameReady{false},
        m_secPerFrame(timePerFrame),
        m_timeSinceLastUpdate{} {
    m_nextFrame.resize(getSizeInBytes());
  }

  virtual void updateImage(const std::vector<uint8_t>& data) override {
    std::unique_lock<std::mutex> mlock(m_frameMutex);
    assert(data.size() == m_nextFrame.size());
    std::copy(std::begin(data), std::end(data), std::begin(m_nextFrame));
    m_frameReady = true;
  }

  bool frameRenderingQueued(const Ogre::FrameEvent& evt) {
    m_timeSinceLastUpdate += evt.timeSinceLastFrame;
    if (m_timeSinceLastUpdate > m_secPerFrame) {
      if (m_frameReady) {
        std::unique_lock<std::mutex> mlock(m_frameMutex);
        DynamicTexture::updateImage(m_nextFrame);
        m_timeSinceLastUpdate = 0.0f;
        m_frameReady = false;
      }
    }
  }

 private:
  std::mutex m_frameMutex;
  std::vector<uint8_t> m_nextFrame;
  std::atomic<bool> m_frameReady;

  float m_secPerFrame;
  float m_timeSinceLastUpdate;
};

#endif  // OGRE_SAMPLE_VIDEOTEXTURE_H
