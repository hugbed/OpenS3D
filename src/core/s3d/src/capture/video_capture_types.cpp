#include "s3d/capture/video_capture_types.h"
#include "s3d/capture/VideoFrame.h"

VideoCaptureFormat::VideoCaptureFormat()
  : frameSize{}
  , frameRate{0.0f}
  , pixelFormat{VideoPixelFormat::UNKNOWN}
{}

VideoCaptureFormat::VideoCaptureFormat(Size &frameSize,
                                       float frameRate,
                                       VideoPixelFormat pixelFormat)
  : frameSize{frameSize}
  , frameRate{frameRate}
  , pixelFormat{pixelFormat}
{
}

size_t VideoCaptureFormat::ImageAllocationSize() const {
  return VideoFrame::AllocationSize(pixelFormat, frameSize);
}