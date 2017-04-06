//
// Created by bedh2102 on 06/04/17.
//

#ifndef PROJECT_VIDEO_CAPTURE_TYPES_H_H
#define PROJECT_VIDEO_CAPTURE_TYPES_H_H

#include "video_types.h"

class VideoCaptureFormat {
public:
  constexpr VideoCaptureFormat() noexcept;
  constexpr VideoCaptureFormat(gfx::Size &frameSize, float frameRate, VideoPixelFormat pixelFormat) noexcept;

  constexpr size_t ImageAllocationSize() const;

private:
  Size frameSize;
  float frameRate;
  VideoPixelFormat pixelFormat;
};

#endif //PROJECT_VIDEO_CAPTURE_TYPES_H_H
