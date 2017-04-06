//
// Created by bedh2102 on 06/04/17.
//

#ifndef PROJECT_VIDEOFRAME_H
#define PROJECT_VIDEOFRAME_H

class Size;

class VideoFrame {
  static size_t AllocationSize(VideoPixelFormat format, const Size& size);
  static size_t NumPlanes(VideoPixelFormat format);
  static Size PlaneSize(VideoPixelFormat format, size_t plane, const Size& size);
  static bool RequireEvenSizeAllocation(VideoPixelFormat format);
  static Size VideoFrame::SampleSize(VideoPixelFormat format, size_t plane);
};

#endif //PROJECT_VIDEOFRAME_H
