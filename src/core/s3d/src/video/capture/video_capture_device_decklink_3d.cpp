
// Copyright 2014 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#include "s3d/video/capture/video_capture_device_decklink_3d.h"
#include "s3d/video/capture/decklink.h"

// inspiration: https://forum.blackmagicdesign.com/viewtopic.php?f=12&t=33269
// todo: should verify which pixel format we need (BGRA or RGBA)
// todo: reuse this from not 3D
class RGB8VideoFrame : public IDeckLinkVideoFrame {
 public:
  constexpr static const int kNbBytesPixel = 4;

  RGB8VideoFrame(long width, long height, BMDPixelFormat pixelFormat, BMDFrameFlags flags);

  // override these methods for virtual
  virtual long GetWidth();
  virtual long GetHeight();
  virtual long GetRowBytes();
  virtual BMDPixelFormat GetPixelFormat();
  virtual BMDFrameFlags GetFlags();
  virtual HRESULT GetBytes(/* out */ void** buffer);

  void resize(long width, long height) {
    width_ = width;
    height_ = height;
    bufferPointer_.resize(static_cast<size_t>(width_ * height_ * kNbBytesPixel));
  }

  // Dummy implementations of remaining virtual methods
  virtual HRESULT GetTimecode(/* in */ BMDTimecodeFormat format,
                              /* out */ IDeckLinkTimecode** timecode) {
    return E_NOINTERFACE;
  }

  virtual HRESULT GetAncillaryData(/* out */ IDeckLinkVideoFrameAncillary** ancillary) {
    return E_NOINTERFACE;
  }

  //
  // IUnknown interface (dummy implementation)
  //
  virtual HRESULT QueryInterface(REFIID iid, LPVOID* ppv) { return E_NOINTERFACE; }
  virtual ULONG AddRef() { return 1; }
  virtual ULONG Release() { return 1; }

 private:
  long width_;
  long height_;
  BMDPixelFormat pixelFormat_;
  BMDFrameFlags frameFlags_;
  std::vector<uint8_t> bufferPointer_;
};

RGB8VideoFrame::RGB8VideoFrame(long width,
                               long height,
                               BMDPixelFormat pixelFormat,
                               BMDFrameFlags frameFlags)
    : width_{width}, height_{height}, pixelFormat_{pixelFormat}, frameFlags_{frameFlags} {
  // this pointer size is only valid for bmdFormat8BitYUV
  switch (pixelFormat_) {
    case bmdFormat8BitBGRA:
      bufferPointer_.resize(static_cast<size_t>(width_ * kNbBytesPixel * height_));
      break;
    default:
      break;
  }
}

long RGB8VideoFrame::GetWidth() {
  return width_;
}

long RGB8VideoFrame::GetHeight() {
  return height_;
}

long RGB8VideoFrame::GetRowBytes() {
  return width_ * kNbBytesPixel;
}

BMDPixelFormat RGB8VideoFrame::GetPixelFormat() {
  return pixelFormat_;
}

BMDFrameFlags RGB8VideoFrame::GetFlags() {
  return frameFlags_;
}

HRESULT RGB8VideoFrame::GetBytes(/* out */ void** buffer) {
  *buffer = static_cast<void*>(&bufferPointer_[0]);
  return S_OK;
}

// todo: better consistency between DeckLink and decklink (camel case?)
// todo: this could be shared with VideoCaptureDevice3D and could have either a 3D or 2D
class DecklinkCaptureDelegate3D : public IDeckLinkInputCallback {
 public:
  DecklinkCaptureDelegate3D(const VideoCaptureDeviceDescriptor& device_descriptor,
                            VideoCaptureDeviceDecklink3D* frameReceiver)
      : device_descriptor_{device_descriptor},
        frameReceiver_{frameReceiver},
        rgbFrameLeft_{new RGB8VideoFrame(1920, 1080, bmdFormat8BitBGRA, bmdFrameFlagDefault)},
        rgbFrameRight_{new RGB8VideoFrame(1920, 1080, bmdFormat8BitBGRA, bmdFrameFlagDefault)} {}

  ~DecklinkCaptureDelegate3D() override = default;

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
  VideoCaptureDeviceDecklink3D* frameReceiver_;
  // This is used to control the video capturing device input interface.
  SmartDecklinkPtr<IDeckLinkInput> deckLinkInput_;
  // |decklink_| represents a physical device attached to the host.
  SmartDecklinkPtr<IDeckLink> deckLink_;

  SmartDecklinkPtr<IDeckLinkVideoConversion> videoConversion_;
  std::unique_ptr<RGB8VideoFrame> rgbFrameLeft_;
  std::unique_ptr<RGB8VideoFrame> rgbFrameRight_;

  size_t refCount_;

  std::chrono::high_resolution_clock::time_point firstRefTime_;
};

void DecklinkCaptureDelegate3D::AllocateAndStart(const VideoCaptureFormat& params) {
  IDeckLinkIterator* deckLinkIterator = CreateDeckLinkIteratorInstance();
  if (deckLinkIterator == nullptr) {
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

  // todo: set from capture format
  BMDDisplayMode displayMode = bmdModeHD1080p30;
  BMDPixelFormat pixelFormat = bmdFormat8BitYUV;
  //  BMDVideoInputFlags videoInputFlag = bmdVideoInputFlagDefault;  // 2D only
  BMDVideoInputFlags videoInputFlag = bmdVideoInputDualStream3D;  // enable 3D

  HRESULT result = deckLinkInput->DoesSupportVideoMode(displayMode, pixelFormat, videoInputFlag,
                                                       &displayModeSupported, NULL);
  if (result != S_OK) {
    SendErrorString("Display mode not supported");
    return;
  }

  // todo: 3D settings for VideoCapture3D
  auto decklinkConfiguration = make_decklink_ptr<IDeckLinkConfiguration>(deckLink);
  if (decklinkConfiguration == nullptr) {
    SendErrorString("Error creating configuration interface");
    return;
  }
  if (decklinkConfiguration->SetFlag(bmdDeckLinkConfigSDIInput3DPayloadOverride, true) != S_OK) {
    SendErrorString("Cannot set 3D payload override flag");
    return;
  }

  // create decklink conversion for YUV -> RGB
  videoConversion_.reset(CreateVideoConversionInstance());

  // Enable Video Input
  if (deckLinkInput->EnableVideoInput(displayMode, pixelFormat, videoInputFlag) != S_OK) {
    SendErrorString("Cannot enable video input");
    return;
  }

  if (deckLinkInput->StartStreams() != S_OK) {
    SendErrorString("Cannot start capture stream");
    return;
  }

  deckLink_.reset(deckLink);
  deckLinkInput_.swap(deckLinkInput);
}
HRESULT DecklinkCaptureDelegate3D::VideoInputFormatChanged(
    BMDVideoInputFormatChangedEvents /*notification_events*/,
    IDeckLinkDisplayMode* /*new_display_mode*/,
    BMDDetectedVideoInputFormatFlags /*detected_signal_flags*/) {
  return S_OK;
}
HRESULT DecklinkCaptureDelegate3D::VideoInputFrameArrived(
    IDeckLinkVideoInputFrame* videoFrameLeft,
    IDeckLinkAudioInputPacket* /*audio_packet*/) {
  // 3D extension
  auto threeDExtension = make_decklink_ptr<IDeckLinkVideoFrame3DExtensions>(videoFrameLeft);
  if (threeDExtension == nullptr) {
    SendErrorString("Error creating 3D extension");
    return S_FALSE;
  }

  if (videoFrameLeft->GetFlags() & bmdFrameHasNoInputSource) {
    SendErrorString("Left frame, no input signal");
    return S_FALSE;
  }

  // get left frame
  rgbFrameLeft_->resize(videoFrameLeft->GetWidth(), videoFrameLeft->GetHeight());
  videoConversion_->ConvertFrame(videoFrameLeft,
                                 static_cast<IDeckLinkVideoFrame*>(rgbFrameLeft_.get()));
  uint8_t* video_data_left = nullptr;
  rgbFrameLeft_->GetBytes(reinterpret_cast<void**>(&video_data_left));

  // get right frame
  IDeckLinkVideoFrame* rightEyeFrameRaw = NULL;
  threeDExtension->GetFrameForRightEye(&rightEyeFrameRaw);
  SmartDecklinkPtr<IDeckLinkVideoFrame> videoFrameRight{rightEyeFrameRaw};

  if (videoFrameRight == nullptr) {
    SendErrorString("No right frame detected");
  }

  rgbFrameRight_->resize(videoFrameRight->GetWidth(), videoFrameRight->GetHeight());
  videoConversion_->ConvertFrame(videoFrameRight.get(),
                                 static_cast<IDeckLinkVideoFrame*>(rgbFrameRight_.get()));
  uint8_t* video_data_right = nullptr;
  rgbFrameRight_->GetBytes(reinterpret_cast<void**>(&video_data_right));

  VideoPixelFormat pixelFormat = VideoPixelFormat::UNKNOWN;
  switch (rgbFrameLeft_->GetPixelFormat()) {
    case bmdFormat8BitYUV:  // A.k.a. '2vuy';
      pixelFormat = VideoPixelFormat::UYVY;
      break;
    case bmdFormat8BitBGRA:
      pixelFormat = VideoPixelFormat::ARGB;
      break;
    default:
      SendErrorString("Unsupported pixel format");
      break;
  }

  const VideoCaptureFormat capture_format(
      Size(rgbFrameLeft_->GetWidth(),
           rgbFrameLeft_->GetHeight()),  // todo: cast or something (long -> int)
      0.0f,                              // Frame rate is not needed for captured data callback.
      pixelFormat);  // todo: 3D flag on capture_format is redundant since there are 2 interfaces
                     // now
  //  auto now = std::chrono::high_resolution_clock::now();
  //  if (firstRefTime_ == 0ms)
  //  firstRefTime_ = now;
  //  base::AutoLock lock(lock_);
  if (frameReceiver_) {
    //    // todo: put this in another file somewhere
    //    constexpr const int kMicrosecondsPerSecond = 1000000;
    //
    //    const BMDTimeScale micros_time_scale = kMicrosecondsPerSecond;
    //    BMDTimeValue frame_time;
    //    BMDTimeValue frame_duration;
    //    std::chrono::duration timestamp; // time delay since the last frame
    //    if (SUCCEEDED(video_frame->GetStreamTime(&frame_time, &frame_duration,
    //    micros_time_scale))) {
    //      timestamp = std::chrono::microseconds(frame_time);
    //    } else {
    //      timestamp = now - firstRefTime_;
    //    }

    frameReceiver_->OnIncomingCapturedData(
        {video_data_left, rgbFrameLeft_->GetRowBytes() * rgbFrameLeft_->GetHeight()},
        {video_data_right, rgbFrameRight_->GetRowBytes() * rgbFrameRight_->GetHeight()},
        capture_format);
    // std::chrono::duration_cast<std::chrono::microseconds>(now - firstRefTime_));  // todo:
    // include timestamp
  }

  return S_OK;
}

void DecklinkCaptureDelegate3D::StopAndDeAllocate() {
  if (!deckLinkInput_.get())
    return;
  if (deckLinkInput_->StopStreams() != S_OK)
    SendLogString("Problem stopping capture.");
  deckLinkInput_->SetCallback(nullptr);
  deckLinkInput_->DisableVideoInput();
  deckLinkInput_.reset(nullptr);
  deckLink_.reset(nullptr);
}

void DecklinkCaptureDelegate3D::SendErrorString(const std::string& reason) {
  if (frameReceiver_ != nullptr) {
    frameReceiver_->SendErrorString(reason);
  }
}

void DecklinkCaptureDelegate3D::SendLogString(const std::string& message) {
  if (frameReceiver_ != nullptr) {
    frameReceiver_->SendLogString(message);
  }
}

void VideoCaptureDeviceDecklink3D::OnIncomingCapturedData(gsl::span<const uint8_t> imageLeft,
                                                          gsl::span<const uint8_t> imageRight,
                                                          const VideoCaptureFormat& frameFormat) {
  if (client_ != nullptr) {
    client_->OnIncomingCapturedData({imageRight, imageLeft}, frameFormat);
  }
}

VideoCaptureDeviceDecklink3D::VideoCaptureDeviceDecklink3D(
    const VideoCaptureDeviceDescriptor& deviceDescriptor)
    : captureDelegate_{new DecklinkCaptureDelegate3D{deviceDescriptor, this}} {}

gsl::owner<VideoCaptureDevice*> VideoCaptureDeviceDecklink3D::clone() {
  return new VideoCaptureDeviceDecklink3D(captureDelegate_->getDeviceDescriptor());
}

VideoCaptureDeviceDecklink3D::~VideoCaptureDeviceDecklink3D() {
  VideoCaptureDeviceDecklink3D::StopAndDeAllocate();
}

void VideoCaptureDeviceDecklink3D::AllocateAndStart(
    const VideoCaptureFormat& format,
    std::unique_ptr<VideoCaptureDevice::Client> client) {
  client_ = std::move(client);
  // todo: should verify that format is supported
  // todo: should get image size from capture delegate
  captureDelegate_->AllocateAndStart(format);
}

void VideoCaptureDeviceDecklink3D::StopAndDeAllocate() {
  captureDelegate_->StopAndDeAllocate();
}

void VideoCaptureDeviceDecklink3D::SendErrorString(const std::string& reason) {
  if (client_ != nullptr) {
    client_->OnError(reason);
  }
}

void VideoCaptureDeviceDecklink3D::SendLogString(const std::string& message) {
  if (client_ != nullptr) {
    client_->OnLog(message);
  }
}
