//
// Created by bedh2102 on 06/04/17.
//

#include "s3d/video/VideoFrame.h"

size_t VideoFrame::AllocationSize(VideoPixelFormat format, const Size &size) {
    switch (format) {
        case VideoPixelFormat::ARGB:
            return static_cast<size_t>(4 * size.getWidth() * size.getHeight());
        case VideoPixelFormat::UYVY:
            return static_cast<size_t>(2 * size.getWidth() * size.getHeight());
        case VideoPixelFormat::UNKNOWN:
            return 0;
    }
}
