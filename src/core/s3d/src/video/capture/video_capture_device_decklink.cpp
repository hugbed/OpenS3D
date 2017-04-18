// Copyright 2014 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#include "s3d/video/capture/video_capture_device_decklink.h"
#include "s3d/video/capture/video_capture_device_factory.h"
#include "s3d/video/capture/video_capture_types.h"

#include <decklink_sdk/DeckLinkAPI.h>

// based on: http://en.cppreference.com/w/cpp/language/constexpr
// Utilities for smart handling of DeckLink pointers
struct DecklinkPtrDeleter {
  template <typename T>
  void operator()(T* ptr) {
    if (ptr)
      ptr->Release();
  }
};

template <typename T>
struct decklink_guid {
  // general case not defined
};

template <>
struct decklink_guid<IDeckLinkInput> {
  constexpr static const REFIID value = IID_IDeckLinkInput;
};

template <>
struct decklink_guid<IDeckLink> {
  constexpr static const REFIID value = IID_IDeckLink;
};

template <>
struct decklink_guid<IDeckLinkConfiguration> {
  constexpr static const REFIID value = IID_IDeckLinkConfiguration;
};

template <class U, class T>
std::unique_ptr<U, DecklinkPtrDeleter> make_decklink_ptr(T const& src) {
  if (!src)
    return {};
  U* r = nullptr;
  if (src->QueryInterface(decklink_guid<U>::value, (void**)&r) != S_OK)
    return {};
  return {r, {}};
}

template <class T>
using SmartDecklinkPtr = std::unique_ptr<T, DecklinkPtrDeleter>;

// use it this way
// auto decklLink = make_decklink_ptr<IDeckLinkInput>(m_deckLinkInput);

// todo: this could be shared with VideoCaptureDevice3D and could have either a 3D or 2D
class DeckLinkCaptureDelegate : public IDeckLinkInputCallback {
 public:
  DeckLinkCaptureDelegate(const VideoCaptureDeviceDescriptor& device_descriptor,
                          VideoCaptureDeviceDecklink* frameReceiver)
      : device_descriptor_{device_descriptor} {}

  ~DeckLinkCaptureDelegate() override = default;

  void AllocateAndStart(const VideoCaptureFormat& params);
  void StopAndDeAllocate();

  const VideoCaptureDeviceDescriptor& getDeviceDescriptor() { return device_descriptor_; }

 private:
  // IDeckLinkInputCallback interface implementation.
  HRESULT VideoInputFormatChanged(BMDVideoInputFormatChangedEvents notification_events,
                                  IDeckLinkDisplayMode* new_display_mode,
                                  BMDDetectedVideoInputFormatFlags detected_signal_flags) override;
  HRESULT VideoInputFrameArrived(IDeckLinkVideoInputFrame* video_frame,
                                 IDeckLinkAudioInputPacket* audio_packet) override;

  ULONG STDMETHODCALLTYPE AddRef(void) override { return ++refCount_; }
  ULONG STDMETHODCALLTYPE Release(void) override { return --refCount_; }  // accepted memory leak
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID* ppv) override {
    return E_NOINTERFACE;
  }

  // Forwarder to VideoCaptureDeviceDeckLinkMac::SendErrorString().
  void SendErrorString(const std::string& reason);
  // Forwarder to VideoCaptureDeviceDeckLinkMac::SendLogString().
  void SendLogString(const std::string& message);

  const VideoCaptureDeviceDescriptor device_descriptor_;

  // Weak reference to the captured frames client, used also for error messages
  // and logging. Initialized on construction and used until cleared by calling
  // ResetVideoCaptureDeviceReference().
  VideoCaptureDeviceDecklink* frameReceiver_;
  // This is used to control the video capturing device input interface.
  SmartDecklinkPtr<IDeckLinkInput> deckLinkInput_;
  // |decklink_| represents a physical device attached to the host.
  SmartDecklinkPtr<IDeckLink> deckLink_;

  size_t refCount_;

  //  std::chrono::time_point firstRefTime_;
};

void DeckLinkCaptureDelegate::AllocateAndStart(const VideoCaptureFormat& params) {
  IDeckLinkIterator* deckLinkIterator = CreateDeckLinkIteratorInstance();
  if (!deckLinkIterator) {
    SendErrorString("Error creating DeckLink iterator");
    return;
  }

  IDeckLink* deckLink;
  if (deckLinkIterator->Next(&deckLink) != S_OK) {
    SendErrorString("No DeckLink device found");
    return;
  }

  auto deckLinkInput = make_decklink_ptr<IDeckLinkInput>(deckLink);
  if (deckLinkInput == nullptr) {
    SendErrorString("Error querying input interface");
    return;
  }
  deckLinkInput->SetCallback(this);

  // Check if display mode is supported
  BMDDisplayModeSupport displayModeSupported;

  BMDDisplayMode displayMode = bmdModeHD720p60;
  BMDPixelFormat pixelFormat = bmdFormat8BitYUV;
  BMDVideoInputFlags videoInputFlag = bmdVideoInputDualStream3D;  // enable 3D
  //    BMDVideoInputFlags videoInputFlag = bmdVideoInputFlagDefault; // 2D only

  HRESULT result = deckLinkInput->DoesSupportVideoMode(displayMode, pixelFormat, videoInputFlag,
                                                       &displayModeSupported, NULL);
  if (result != S_OK) {
    SendErrorString("DeckLinkCapture::init(): Display mode not supported");
    return;
  }

  // 3D settings
  auto decklinkConfiguration = make_decklink_ptr<IDeckLinkConfiguration>(deckLink);
  if (decklinkConfiguration == nullptr) {
    SendErrorString("DeckLinkCapture::init(): QueryInterface(Configuration) failed");
    return;
  }
  if (decklinkConfiguration->SetFlag(bmdDeckLinkConfigSDIInput3DPayloadOverride, true) != S_OK) {
    SendErrorString("DeckLinkCapture::init(): Overide 3D payload failed");
    return;
  }

  // Enable Video Input
  if (deckLinkInput->EnableVideoInput(displayMode, pixelFormat, videoInputFlag) != S_OK) {
    SendErrorString("DeckLinkCapture::init(): EnableVideoInput() failed");
    return;
  }

  if (deckLinkInput->StartStreams() != S_OK) {
    SendErrorString("DeckLinkCapture::init(): StartStreams() failed");
    return;
  }

  deckLink_.reset(deckLink);
  deckLinkInput_.swap(deckLinkInput);
}
HRESULT DeckLinkCaptureDelegate::VideoInputFormatChanged(
    BMDVideoInputFormatChangedEvents /*notification_events*/,
    IDeckLinkDisplayMode* /*new_display_mode*/,
    BMDDetectedVideoInputFormatFlags /*detected_signal_flags*/) {
  return S_OK;
}
HRESULT DeckLinkCaptureDelegate::VideoInputFrameArrived(
    IDeckLinkVideoInputFrame* video_frame,
    IDeckLinkAudioInputPacket* /*audio_packet*/) {
  uint8_t* video_data = nullptr;
  video_frame->GetBytes(reinterpret_cast<void**>(&video_data));
  VideoPixelFormat pixelFormat = VideoPixelFormat::UNKNOWN;
  switch (video_frame->GetPixelFormat()) {
    case bmdFormat8BitYUV:  // A.k.a. '2vuy';
      pixelFormat = VideoPixelFormat::UYVY;
      break;
    case bmdFormat8BitARGB:
      pixelFormat = VideoPixelFormat::ARGB;
      break;
    default:
      SendErrorString("Unsupported pixel format");
      break;
  }

  const VideoCaptureFormat capture_format(
      Size(video_frame->GetWidth(), video_frame->GetHeight()),
      0.0f,  // Frame rate is not needed for captured data callback.
      pixelFormat);
  //  auto now = std::chrono::high_resolution_clock::now();
  //  if (firstRefTime_ == 0ms)
  //    firstRefTime_ = now;
  //  base::AutoLock lock(lock_);
  if (frameReceiver_) {
    //    const BMDTimeScale micros_time_scale = base::Time::kMicrosecondsPerSecond;
    //    BMDTimeValue frame_time;
    //    BMDTimeValue frame_duration;
    //    std::chrono::time_point timestamp;
    //    if (SUCCEEDED(video_frame->GetStreamTime(&frame_time, &frame_duration,
    //                                             micros_time_scale))) {
    //      timestamp = frame_time;
    //    } else {
    //      timestamp = now - firstRefTime_;
    //    }
    frameReceiver_->OnIncomingCapturedData(
        {video_data, video_frame->GetRowBytes() * video_frame->GetHeight()},
        capture_format);  // todo: include timestamp
                          //        0,  // Rotation.
                          //        now, timestamp);
  }
  return S_OK;
}

void DeckLinkCaptureDelegate::StopAndDeAllocate() {
  if (!deckLinkInput_.get())
    return;
  if (deckLinkInput_->StopStreams() != S_OK)
    SendLogString("Problem stopping capture.");
  deckLinkInput_->SetCallback(nullptr);
  deckLinkInput_->DisableVideoInput();
  deckLinkInput_.reset(nullptr);
  deckLink_.reset(nullptr);
}

void DeckLinkCaptureDelegate::SendErrorString(const std::string& reason) {
  if (frameReceiver_ != nullptr) {
    frameReceiver_->SendErrorString(reason);
  }
}

void DeckLinkCaptureDelegate::SendLogString(const std::string& message) {
  if (frameReceiver_ != nullptr) {
    frameReceiver_->SendLogString(message);
  }
}

void VideoCaptureDeviceDecklink::OnIncomingCapturedData(gsl::span<const uint8_t> data,
                                                        const VideoCaptureFormat& frameFormat) {
  if (client_) {
    client_->OnIncomingCapturedData(data, frameFormat);
  }
}

VideoCaptureDeviceDecklink::~VideoCaptureDeviceDecklink() {
  VideoCaptureDeviceDecklink::StopAndDeAllocate();
}

void VideoCaptureDeviceDecklink::AllocateAndStart(
    const VideoCaptureFormat& format,
    std::unique_ptr<VideoCaptureDevice::Client> client) {
  client_ = std::move(client);
  captureDelegate_->AllocateAndStart(format);
}

void VideoCaptureDeviceDecklink::StopAndDeAllocate() {
  captureDelegate_->StopAndDeAllocate();
}

VideoCaptureDeviceDecklink::VideoCaptureDeviceDecklink(
    const VideoCaptureDeviceDescriptor& deviceDescriptor)
    : captureDelegate_{new DeckLinkCaptureDelegate{deviceDescriptor, this}} {}

gsl::owner<VideoCaptureDevice*> VideoCaptureDeviceDecklink::clone() {
  return new VideoCaptureDeviceDecklink(captureDelegate_->getDeviceDescriptor());
}

void VideoCaptureDeviceDecklink::SendErrorString(const std::string& reason) {
  client_->OnError(reason);
}

void VideoCaptureDeviceDecklink::SendLogString(const std::string& message) {
  client_->OnLog(message);
}
