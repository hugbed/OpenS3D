#include "s3d/video/capture/video_capture_types.h"
#include "s3d/video/video_frame.h"

#include <algorithm>

VideoCaptureFormat::VideoCaptureFormat()
    : frameSize{},
      frameRate{0.0f},
      pixelFormat{VideoPixelFormat::UNKNOWN},
      stereo3D{false} {}

VideoCaptureFormat::VideoCaptureFormat(Size frameSize,
                                       float frameRate,
                                       VideoPixelFormat pixelFormat)
    : frameSize{frameSize},
      frameRate{frameRate},
      pixelFormat{pixelFormat},
      stereo3D{false} {}

VideoCaptureFormat::VideoCaptureFormat(Size frameSize,
                                       float frameRate,
                                       VideoPixelFormat pixelFormat,
                                       bool stereo3D)
    : frameSize{frameSize},
      frameRate{frameRate},
      pixelFormat{pixelFormat},
      stereo3D{stereo3D} {}

size_t VideoCaptureFormat::ImageAllocationSize() const {
  auto sizeInBytes = VideoFrame::AllocationSize(pixelFormat, frameSize);
  return stereo3D ? sizeInBytes : 2 * sizeInBytes;
}