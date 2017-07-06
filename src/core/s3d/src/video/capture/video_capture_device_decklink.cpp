// Copyright 2014 The Chromium Authors. All rights reserved.
// Inspired by Chromium video capture interface
// Simplified and stripped from internal base code

#include "s3d/video/capture/video_capture_device_decklink.h"
#include "s3d/video/capture/decklink.h"

// inspiration: https://forum.blackmagicdesign.com/viewtopic.php?f=12&t=33269
// todo: should verify which pixel format we need (BGRA or RGBA)
// todo: should test int64_t instead of long
class RGB8VideoFrame : public IDeckLinkVideoFrame {
 public:
  static const int kNbBytesPixel = 4;
  static const BMDPixelFormat kPixelFormat = bmdFormat8BitBGRA;

  RGB8VideoFrame(int64_t width, int64_t height, BMDPixelFormat pixelFormat, BMDFrameFlags flags);

  // override these methods for virtual
  int64_t GetWidth() override;
  int64_t GetHeight() override;
  int64_t GetRowBytes() override;
  BMDPixelFormat GetPixelFormat() override;
  BMDFrameFlags GetFlags() override;
  HRESULT GetBytes(/* out */ void** buffer) override;

  void resize(int64_t width, int64_t height);

  // Dummy implementations of remaining virtual methods
  HRESULT GetTimecode(BMDTimecodeFormat /*format*/, IDeckLinkTimecode** /*timecode*/) override {
    return E_NOINTERFACE;
  }

  HRESULT GetAncillaryData(IDeckLinkVideoFrameAncillary** /*ancillary*/) override {
    return E_NOINTERFACE;
  }

  //
  // IUnknown interface (dummy implementation)
  //
  HRESULT QueryInterface(REFIID /*iid*/, LPVOID* /*ppv*/) override { return E_NOINTERFACE; }
  ULONG AddRef() override { return 1; }
  ULONG Release() override { return 1; }

 private:
  int64_t width_;
  int64_t height_;
  BMDPixelFormat pixelFormat_;
  BMDFrameFlags frameFlags_;
  std::vector<uint8_t> bufferPointer_;
};

RGB8VideoFrame::RGB8VideoFrame(int64_t width,
                               int64_t height,
                               BMDPixelFormat pixelFormat,
                               BMDFrameFlags flags)
    : width_{width}, height_{height}, pixelFormat_{pixelFormat}, frameFlags_{flags} {
  assert(pixelFormat == kPixelFormat);
  bufferPointer_.resize(static_cast<size_t>(width_ * kNbBytesPixel * height_));
}

int64_t RGB8VideoFrame::GetWidth() {
  return width_;
}

int64_t RGB8VideoFrame::GetHeight() {
  return height_;
}

int64_t RGB8VideoFrame::GetRowBytes() {
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

void RGB8VideoFrame::resize(int64_t width, int64_t height) {
  width_ = width;
  height_ = height;
  bufferPointer_.resize(static_cast<size_t>(width_ * height_ * kNbBytesPixel));
}

// todo: better consistency between DeckLink and decklink (camel case?)
class DecklinkCaptureDelegate : public IDeckLinkInputCallback {
 public:
  DecklinkCaptureDelegate(const VideoCaptureDeviceDescriptor& device_descriptor,
                          VideoCaptureDeviceDecklink* frameReceiver);

  ~DecklinkCaptureDelegate() override = default;

  void AllocateAndStart(const VideoCaptureFormat& params);
  void StopAndDeAllocate();

  const VideoCaptureDeviceDescriptor& getDeviceDescriptor() { return device_descriptor_; }

  static bool supportedFormat(const VideoCaptureFormat& format);

  void ResetVideoCaptureDeviceReference() { frameReceiver_ = nullptr; }

 private:
  // IDeckLinkInputCallback interface implementation.
  HRESULT VideoInputFormatChanged(BMDVideoInputFormatChangedEvents notification_events,
                                  IDeckLinkDisplayMode* new_display_mode,
                                  BMDDetectedVideoInputFormatFlags detected_signal_flags) override;
  HRESULT VideoInputFrameArrived(IDeckLinkVideoInputFrame* videoFrameLeft,
                                 IDeckLinkAudioInputPacket* audio_packet) override;

  ULONG STDMETHODCALLTYPE AddRef() override { return ++refCount_; }
  ULONG STDMETHODCALLTYPE Release() override { return --refCount_; }  // accepted memory leak
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID /*iid*/, LPVOID* /*ppv*/) override {
    return E_NOINTERFACE;
  }

  // Forwarder to VideoCaptureDeviceDeckLinkMac::SendErrorString().
  void SendErrorString(const std::string& reason);
  // Forwarder to VideoCaptureDeviceDeckLinkMac::SendLogString().
  void SendLogString(const std::string& message);

  const VideoCaptureDeviceDescriptor device_descriptor_;
  VideoCaptureFormat captureFormat_;

  // Weak reference to the captured frames client, used also for error messages
  // and logging. Initialized on construction and used until cleared by calling
  // ResetVideoCaptureDeviceReference().
  VideoCaptureDeviceDecklink* frameReceiver_;
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

DecklinkCaptureDelegate::DecklinkCaptureDelegate(
    const VideoCaptureDeviceDescriptor& device_descriptor,
    VideoCaptureDeviceDecklink* frameReceiver)
    : device_descriptor_{device_descriptor},
      captureFormat_{{0, 0}, 0.0f, VideoPixelFormat::UNKNOWN},
      frameReceiver_{frameReceiver},
      rgbFrameLeft_{new RGB8VideoFrame(0, 0, RGB8VideoFrame::kPixelFormat, bmdFrameFlagDefault)},
      rgbFrameRight_{new RGB8VideoFrame(0, 0, RGB8VideoFrame::kPixelFormat, bmdFrameFlagDefault)} {}

void DecklinkCaptureDelegate::AllocateAndStart(const VideoCaptureFormat& params) {
  // Only 1920x1080, 30fps, BGRA, 2D or 3D supported
  // todo: is it BGRA or ARGB?
  if (!supportedFormat(params)) {
    // todo: this seems like another type of exception, or a bug?
    throw VideoCaptureDeviceAllocationException("Requested format not supported");
  }

  IDeckLinkIterator* deckLinkIterator = CreateDeckLinkIteratorInstance();
  if (deckLinkIterator == nullptr) {
    throw VideoCaptureDeviceAllocationException("Error creating DeckLink iterator");
  }

  IDeckLink* deckLink;
  if (deckLinkIterator->Next(&deckLink) != S_OK) {
    throw VideoCaptureDeviceAllocationException("No DeckLink device found");
  }

  auto deckLinkInput = make_decklink_ptr<IDeckLinkInput>(deckLink);
  deckLinkInput->SetCallback(this);

  // Check if display mode is supported
  BMDDisplayModeSupport displayModeSupported;

  // todo: set from capture format
  BMDDisplayMode displayMode = bmdModeHD1080p30;
  BMDPixelFormat pixelFormat = bmdFormat8BitYUV;
  BMDVideoInputFlags videoInputFlag =
      params.stereo3D ? bmdVideoInputDualStream3D : bmdVideoInputFlagDefault;

  HRESULT result = deckLinkInput->DoesSupportVideoMode(displayMode, pixelFormat, videoInputFlag,
                                                       &displayModeSupported, nullptr);
  if (result != S_OK) {
    throw VideoCaptureDeviceAllocationException("Display mode not supported");
  }

  // 3D settings for video capture
  if (params.stereo3D) {
    auto decklinkConfiguration = make_decklink_ptr<IDeckLinkConfiguration>(deckLink);
    if (decklinkConfiguration->SetFlag(bmdDeckLinkConfigSDIInput3DPayloadOverride, true) != S_OK) {
      throw VideoCaptureDeviceAllocationException("Cannot set 3D payload override flag");
    }
  }

  // create decklink conversion for YUV -> RGB
  videoConversion_.reset(CreateVideoConversionInstance());

  // Enable Video Input
  if (deckLinkInput->EnableVideoInput(displayMode, pixelFormat, videoInputFlag) != S_OK) {
    throw VideoCaptureDeviceAllocationException("Cannot enable video input");
  }

  // todo: now it's hardcoded to 1080p, 30fps, BGRA
  captureFormat_ = {params.frameSize, params.frameRate, params.pixelFormat, params.stereo3D};
  deckLink_.reset(deckLink);
  deckLinkInput_.swap(deckLinkInput);

  if (deckLinkInput_->StartStreams() != S_OK) {
    throw VideoCaptureDeviceAllocationException("Cannot start capture stream");
  }
}

HRESULT DecklinkCaptureDelegate::VideoInputFormatChanged(
    BMDVideoInputFormatChangedEvents /*notification_events*/,
    IDeckLinkDisplayMode* /*new_display_mode*/,
    BMDDetectedVideoInputFormatFlags /*detected_signal_flags*/) {
  return S_OK;
}

HRESULT DecklinkCaptureDelegate::VideoInputFrameArrived(
    IDeckLinkVideoInputFrame* videoFrameLeft,
    IDeckLinkAudioInputPacket* /*audio_packet*/) {
  if ((videoFrameLeft->GetFlags() & bmdFrameHasNoInputSource) != 0u) {
    SendErrorString("Left frame, no input signal");
    return S_FALSE;
  }

  void* raw_data_left = nullptr;
  void* raw_data_right = nullptr;

  uint8_t* video_data_left = nullptr;
  uint8_t* video_data_right = nullptr;

  // get left frame
  rgbFrameLeft_->resize(videoFrameLeft->GetWidth(), videoFrameLeft->GetHeight());

  videoConversion_->ConvertFrame(videoFrameLeft,
                                 static_cast<IDeckLinkVideoFrame*>(rgbFrameLeft_.get()));
  rgbFrameLeft_->GetBytes(&raw_data_left);
  video_data_left = static_cast<uint8_t*>(raw_data_left);

  if (captureFormat_.stereo3D) {
    auto threeDExtension = make_decklink_ptr<IDeckLinkVideoFrame3DExtensions>(videoFrameLeft);

    // get right frame
    IDeckLinkVideoFrame* rightEyeFrameRaw = nullptr;
    threeDExtension->GetFrameForRightEye(&rightEyeFrameRaw);
    SmartDecklinkPtr<IDeckLinkVideoFrame> videoFrameRight{rightEyeFrameRaw};

    if (videoFrameRight == nullptr) {
      // todo: this is another type of exception, should it be thrown?
      throw VideoCaptureDeviceAllocationException("No right frame detected");
    }

    rgbFrameRight_->resize(videoFrameRight->GetWidth(), videoFrameRight->GetHeight());
    videoConversion_->ConvertFrame(videoFrameRight.get(),
                                   static_cast<IDeckLinkVideoFrame*>(rgbFrameRight_.get()));
    rgbFrameRight_->GetBytes(&raw_data_right);
    video_data_right = static_cast<uint8_t*>(raw_data_right);
  }

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
           rgbFrameLeft_->GetHeight()),  // todo: cast or something (int64_t-> int)
      -1.0f,                             // Frame rate is not needed for captured data callback.
      pixelFormat,
      captureFormat_.stereo3D);

  if (frameReceiver_ != nullptr) {
    if (captureFormat_.stereo3D) {
      frameReceiver_->OnIncomingCapturedData(
          {{video_data_left, rgbFrameLeft_->GetRowBytes() * rgbFrameLeft_->GetHeight()},
           {video_data_right, rgbFrameRight_->GetRowBytes() * rgbFrameRight_->GetHeight()}},
          capture_format, std::chrono::microseconds(0));
    } else {
      frameReceiver_->OnIncomingCapturedData(
          {{video_data_left, rgbFrameLeft_->GetRowBytes() * rgbFrameLeft_->GetHeight()}},
          capture_format, std::chrono::microseconds(0));  // todo: implement timestamp
    }
  }

  return S_OK;
}

void DecklinkCaptureDelegate::StopAndDeAllocate() {
  if (deckLinkInput_ == nullptr) {
    return;
  }
  if (deckLinkInput_->StopStreams() != S_OK) {
    SendLogString("Problem stopping capture.");
  }
  deckLinkInput_->SetCallback(nullptr);
  deckLinkInput_->DisableVideoInput();
  deckLinkInput_.reset(nullptr);
  deckLink_.reset(nullptr);
  ResetVideoCaptureDeviceReference();
}

// static
bool DecklinkCaptureDelegate::supportedFormat(const VideoCaptureFormat& format) {
  return format.frameSize == Size(1920, 1080) && format.frameRate == 30.0f &&
         format.pixelFormat == VideoPixelFormat::ARGB;
}

VideoCaptureDeviceDecklink::VideoCaptureDeviceDecklink(
    const VideoCaptureDeviceDescriptor& deviceDescriptor)
    : captureDelegate_{new DecklinkCaptureDelegate{deviceDescriptor, this}} {}

gsl::owner<VideoCaptureDevice*> VideoCaptureDeviceDecklink::clone() const {
  return new VideoCaptureDeviceDecklink(captureDelegate_->getDeviceDescriptor());
}

VideoCaptureDeviceDecklink::~VideoCaptureDeviceDecklink() {
  VideoCaptureDeviceDecklink::StopAndDeAllocate();
}

void VideoCaptureDeviceDecklink::AllocateAndStart(const VideoCaptureFormat& format,
                                                  VideoCaptureDevice::Client* client) {
  client_ = client;
  // todo: should verify that format is supported
  // todo: should get image size from capture delegate
  captureDelegate_->AllocateAndStart(format);
}

void VideoCaptureDeviceDecklink::StopAndDeAllocate() {
  captureDelegate_->StopAndDeAllocate();
}

void VideoCaptureDeviceDecklink::OnIncomingCapturedData(
    const VideoCaptureDevice::Client::Images& images,
    const VideoCaptureFormat& frameFormat,
    std::chrono::microseconds timestamp) {
  if (client_ != nullptr) {
    client_->OnIncomingCapturedData(images, frameFormat, timestamp);  // todo: implement timestamp
  }
}

VideoCaptureFormat VideoCaptureDeviceDecklink::DefaultFormat() {
  return VideoCaptureFormat(Size(1920, 1080), 30.0f, VideoPixelFormat::ARGB);
}
