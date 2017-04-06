#ifndef _DECK_LINK_CAPTURE_H_
#define _DECK_LINK_CAPTURE_H_

// system includes

// library includes
#include "DeckLinkAPI.h"
#include <ros/ros.h>

// custom includes


// forward declarations
namespace hb {
    class Frame;
}

/*
 * cf. Blackmagic_DeckLink_SDK_9.7.7/Linux/Samples/Capture/
 */
class DeckLinkCapture
  : public IDeckLinkInputCallback
{
  public:
    // enums

    // typedefs
    typedef boost::function<void(hb::Frame* frame)> DeckLinkCaptureCallbackType;
    typedef boost::function<void(hb::Frame* leftFrame, hb::Frame* rightFrame)> DeckLinkCaptureCallback3DType;

    // const static member variables
 
    // static utility functions


    // constructors
    DeckLinkCapture();
    virtual ~DeckLinkCapture();

    // overwritten methods
	virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame*, IDeckLinkAudioInputPacket*);
	virtual ULONG STDMETHODCALLTYPE AddRef(void) { return (ULONG)++m_refCount; }
	virtual ULONG STDMETHODCALLTYPE  Release(void) { return (ULONG)--m_refCount; } // accepted memory leak
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) { return E_NOINTERFACE; }
    virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(BMDVideoInputFormatChangedEvents, IDeckLinkDisplayMode*, BMDDetectedVideoInputFormatFlags) { return S_OK; }

    // methods
    bool init(); // to 1080i60 via HD-SDI
    void setCallback(DeckLinkCaptureCallbackType p_callback);
    void setCallback3D(DeckLinkCaptureCallback3DType p_callback);

    // variables


  private:
    // methods

    // variables
    unsigned long m_refCount;
    unsigned long m_frameCount;
    DeckLinkCaptureCallbackType m_callback;
    DeckLinkCaptureCallback3DType m_callback3D;

    IDeckLinkConfiguration* m_deckLinkConfiguration;
};

#endif // _DECK_LINK_CAPTURE_H_

