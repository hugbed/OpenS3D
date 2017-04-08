#include "s3d/video/capture/video_capture_types.h"
#include "s3d/video/video_frame.h"

VideoCaptureFormat::VideoCaptureFormat()
    : frameSize{}, frameRate{0.0f}, pixelFormat{VideoPixelFormat::UNKNOWN} {}

VideoCaptureFormat::VideoCaptureFormat(Size& frameSize,
                                       float frameRate,
                                       VideoPixelFormat pixelFormat)
    : frameSize{frameSize}, frameRate{frameRate}, pixelFormat{pixelFormat} {}

size_t VideoCaptureFormat::ImageAllocationSize() const {
  return VideoFrame::AllocationSize(pixelFormat, frameSize);
}