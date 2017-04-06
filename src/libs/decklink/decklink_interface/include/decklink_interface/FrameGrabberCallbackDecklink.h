//
// Created by jon on 05/04/17.
//

#ifndef PROJECT_FRAMEGRABBERCALLBACKDECKLINK_H
#define PROJECT_FRAMEGRABBERCALLBACKDECKLINK_H

#include "s3d/utilities/FrameGrabberCallback.h"
#include "decklink_sdk/DeckLinkAPI.h"

class FrameGrabberCallbackDecklink : public s3d::FrameGrabberCallback, public IDeckLinkInputCallback
{
public:
    FrameGrabberCallbackDecklink(s3d::FrameGrabberCallback::callback_type callback)
        : s3d::FrameGrabberCallback(callback)
        , m_refCount(0)
        , m_deckLinkConfiguration(nullptr)
    {}

    virtual ~FrameGrabberCallbackDecklink();

    // overwritten methods
    virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame*, IDeckLinkAudioInputPacket*);
    virtual ULONG STDMETHODCALLTYPE AddRef(void) { return (ULONG)++m_refCount; }
    virtual ULONG STDMETHODCALLTYPE  Release(void) { return (ULONG)--m_refCount; } // accepted memory leak
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) { return E_NOINTERFACE; }
    virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(BMDVideoInputFormatChangedEvents, IDeckLinkDisplayMode*, BMDDetectedVideoInputFormatFlags) { return S_OK; }

    std::vector<uint8_t> m_leftFrameBuffer;
    std::vector<uint8_t> m_rightFrameBuffer;
    unsigned long m_refCount;
    IDeckLinkConfiguration* m_deckLinkConfiguration;
};

#endif //PROJECT_FRAMEGRABBERCALLBACKDECKLINK_H
