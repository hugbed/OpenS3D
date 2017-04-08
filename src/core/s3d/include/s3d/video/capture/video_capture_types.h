//
// Created by bedh2102 on 06/04/17.
//

#ifndef PROJECT_VIDEO_CAPTURE_TYPES_H_H
#define PROJECT_VIDEO_CAPTURE_TYPES_H_H

#include "s3d/video/video_types.h"
#include "s3d/geometry/size.h"

#include <cstddef>

struct VideoCaptureFormat {
    VideoCaptureFormat();
    VideoCaptureFormat(Size &frameSize, float frameRate, VideoPixelFormat pixelFormat);
    size_t ImageAllocationSize() const;

    Size frameSize;
    float frameRate;
    VideoPixelFormat pixelFormat;
};

#endif //PROJECT_VIDEO_CAPTURE_TYPES_H_H
