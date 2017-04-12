//
// Created by jon on 18/03/17.
//

#ifndef OGRE_S3D_PLAYER_VIDEO_TEXTURE_DYNAMIC_TEXTURE_THREAD_SAFE_H
#define OGRE_S3D_PLAYER_VIDEO_TEXTURE_DYNAMIC_TEXTURE_THREAD_SAFE_H

#include "DynamicTexture.hpp"

#include <atomic>
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

  void updateImage(const std::vector<uint8_t>& imageBytes) override {
    std::unique_lock<std::mutex> mlock(m_frameMutex);
    assert(imageBytes.size() == m_nextFrame.size());
    std::copy(std::begin(imageBytes), std::end(imageBytes),
              std::begin(m_nextFrame));
    m_frameReady = true;
  }

  bool frameRenderingQueued(const Ogre::FrameEvent& evt) override {
    m_timeSinceLastUpdate += evt.timeSinceLastFrame;
    if (m_timeSinceLastUpdate > m_secPerFrame) {
      if (m_frameReady) {
        std::unique_lock<std::mutex> mlock(m_frameMutex);
        DynamicTexture::updateImage(m_nextFrame);
        m_timeSinceLastUpdate = 0.0f;
        m_frameReady = false;
      }
    }
    return true;
  }

 private:
  std::mutex m_frameMutex;
  std::vector<uint8_t> m_nextFrame;
  std::atomic<bool> m_frameReady;

  float m_secPerFrame;
  float m_timeSinceLastUpdate;
};

#endif  // OGRE_S3D_PLAYER_VIDEO_TEXTURE_DYNAMIC_TEXTURE_THREAD_SAFE_H
