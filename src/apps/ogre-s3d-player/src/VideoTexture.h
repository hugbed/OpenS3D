//
// Created by jon on 18/03/17.
//

#ifndef OGRE_SAMPLE_VIDEOTEXTURE_H
#define OGRE_SAMPLE_VIDEOTEXTURE_H


#include "DynamicImageTexture.h"

#include <mutex>
#include <chrono>
#include <atomic>

// DynamicImageTexture with frame rate checks
class VideoTexture {
public:
    VideoTexture(DynamicImageTexture* texture, float timePerFrame)
        : m_dynamicTexture(texture)
        , m_frameMutex{}
        , m_nextFrame{}
        , m_frameReady{ false }
        , m_secPerFrame(timePerFrame)
        , m_timeSinceLastUpdate{}
    {
        m_nextFrame.resize(texture->getSizeInBytes());
    }

    void pushFrame(const std::vector<uint8_t> &frame)
    {
        std::unique_lock<std::mutex> mlock(m_frameMutex);
        assert(frame.size() == m_nextFrame.size());
        std::copy(std::begin(frame), std::end(frame), std::begin(m_nextFrame));
        m_frameReady = true;
    }

    bool frameRenderingQueued(const Ogre::FrameEvent & evt)
    {
        m_timeSinceLastUpdate += evt.timeSinceLastFrame;
        if (m_timeSinceLastUpdate > m_secPerFrame) {
            if (m_frameReady) {
                std::unique_lock<std::mutex> mlock(m_frameMutex);
                m_dynamicTexture->updateImage(m_nextFrame);
                m_timeSinceLastUpdate = 0.0f;
                m_frameReady = false;
            }
        }
    }

private:
    DynamicImageTexture* m_dynamicTexture;

    std::mutex m_frameMutex;
    std::vector<uint8_t> m_nextFrame;
    std::atomic<bool> m_frameReady;

    float m_secPerFrame;
    float m_timeSinceLastUpdate;
};


#endif //OGRE_SAMPLE_VIDEOTEXTURE_H
