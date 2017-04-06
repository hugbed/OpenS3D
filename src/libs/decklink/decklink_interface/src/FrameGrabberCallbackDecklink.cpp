//
// Created by jon on 05/04/17.
//

#include "decklink_interface/FrameGrabberCallbackDecklink.h"

// system includes
#include <stdio.h>
#include <memory>

template<class T>
struct Releaser {
    void operator()(T* p) const noexcept
    {
        p->Release();
    }
};

FrameGrabberCallbackDecklink::~FrameGrabberCallbackDecklink()
{
    if (m_deckLinkConfiguration) {
        m_deckLinkConfiguration->Release();
    }
}


HRESULT FrameGrabberCallbackDecklink::VideoInputFrameArrived(IDeckLinkVideoInputFrame* videoFrame, IDeckLinkAudioInputPacket* audioFrame)
{
    if (!videoFrame) {
        return S_OK;
    }

    // If 3D mode is enabled we retreive the 3D extensions interface which gives.
    // us access to the right eye frame by calling GetFrameForRightEye() .
    IDeckLinkVideoFrame* rightEyeFrameRaw = NULL;
    IDeckLinkVideoFrame3DExtensions*  threeDExtensionsRaw = NULL;

    if ( (videoFrame->QueryInterface(IID_IDeckLinkVideoFrame3DExtensions, (void **) &threeDExtensionsRaw) != S_OK) ||
         (threeDExtensionsRaw->GetFrameForRightEye(&rightEyeFrameRaw) != S_OK))
    {
        rightEyeFrameRaw = NULL;
    }

    std::unique_ptr<IDeckLinkVideoFrame, Releaser<IDeckLinkVideoFrame>>
            rightEyeFrame(rightEyeFrameRaw);
    std::unique_ptr<IDeckLinkVideoFrame3DExtensions, Releaser<IDeckLinkVideoFrame3DExtensions>>
            threeDExtensions(threeDExtensionsRaw);

    if (threeDExtensions)
        threeDExtensions->Release();

    if (videoFrame->GetFlags() & bmdFrameHasNoInputSource) {
        // ROS_WARN_THROTTLE(2, "DeckLinkCapture::VideoInputFrameArrived(): Frame %lu: No input signal", m_frameCount);
    }
    else {
//        ROS_INFO_THROTTLE(2, "Frame received (#%lu) - %s - Size: %li bytes",
//                          m_frameCount,
//                // timecodeString != NULL ? timecodeString : "No timecode",
//                          rightEyeFrame != NULL ? "Valid Frame (3D left/right)" : "Valid Frame",
//                          videoFrame->GetRowBytes() * videoFrame->GetHeight());

        void* rawFrame;
        videoFrame->GetBytes(&rawFrame);
        m_leftFrameBuffer.resize(static_cast<size_t>(videoFrame->GetRowBytes() * videoFrame->GetHeight()));
        std::copy(static_cast<char*>(rawFrame), static_cast<char*>(rawFrame) + m_leftFrameBuffer.size(), std::begin(m_leftFrameBuffer));

        if (rightEyeFrame) {
            void* rawRightEyeFrame;
            rightEyeFrame->GetBytes(&rawRightEyeFrame);
            m_rightFrameBuffer.resize(static_cast<size_t>(rightEyeFrame->GetRowBytes() * rightEyeFrame->GetHeight()));
            std::copy(static_cast<char*>(rawRightEyeFrame), static_cast<char*>(rawRightEyeFrame) + m_leftFrameBuffer.size(), std::begin(m_leftFrameBuffer));
            notifyNewFrame(m_leftFrameBuffer, m_rightFrameBuffer);
        } else {
            notifyNewFrame(m_leftFrameBuffer, {});
        }
    }

    if (rightEyeFrame) {
        rightEyeFrame->Release();
    }

    return S_OK;
}

bool FrameGrabberCallbackDecklink::init()
{
    IDeckLinkIterator* deckLinkIterator = CreateDeckLinkIteratorInstance();
    IDeckLink* deckLink;
    if (deckLinkIterator->Next(&deckLink) != S_OK) {
        ROS_ERROR("DeckLinkCapture::init(): deckLinkIterator: No DeckLink device found");
    }

    IDeckLinkInput* deckLinkInput;
    if (deckLink->QueryInterface(IID_IDeckLinkInput, (void**)&deckLinkInput) != S_OK) {
        ROS_ERROR("DeckLinkCapture::init(): QueryInterface(DeckLinkInput) failed");
        return false;
    }
    deckLinkInput->SetCallback(this);

    // Check if display mode is supported
    BMDDisplayModeSupport     displayModeSupported;

    BMDDisplayMode displayMode = bmdModeHD720p60;
    BMDPixelFormat pixelFormat = bmdFormat8BitYUV;
    BMDVideoInputFlags videoInputFlag = bmdVideoInputDualStream3D; // enable 3D
//    BMDVideoInputFlags videoInputFlag = bmdVideoInputFlagDefault; // 2D only

    HRESULT result = deckLinkInput->DoesSupportVideoMode(displayMode, pixelFormat, videoInputFlag, &displayModeSupported, NULL);
    if (result != S_OK) {
        ROS_ERROR("DeckLinkCapture::init(): Display mode not supported");
    }

    // 3D settings
    if (deckLink->QueryInterface(IID_IDeckLinkConfiguration, (void**)&m_deckLinkConfiguration) != S_OK) {
        ROS_ERROR("DeckLinkCapture::init(): QueryInterface(Configuration) failed");
        return false;
    }
    if (m_deckLinkConfiguration->SetFlag(bmdDeckLinkConfigSDIInput3DPayloadOverride, true) != S_OK) {
        ROS_ERROR("DeckLinkCapture::init(): Overide 3D payload failed");
        return false;
    }

    // Enable Video Input
    if (deckLinkInput->EnableVideoInput(displayMode, pixelFormat, videoInputFlag) != S_OK) {
        ROS_ERROR("DeckLinkCapture::init(): EnableVideoInput() failed");
        return false;
    }

    if (deckLinkInput->StartStreams() != S_OK) {
        ROS_ERROR("DeckLinkCapture::init(): StartStreams() failed");
        return false;
    }

    return true;
}
