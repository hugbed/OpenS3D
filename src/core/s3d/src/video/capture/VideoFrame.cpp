//
// Created by bedh2102 on 06/04/17.
//

#include <s3d/geometry/Size.h>
#include "s3d/video/VideoFrame.h"

#include "s3d/utilities/math.h"

size_t VideoFrame::AllocationSize(VideoPixelFormat format, const Size &size) {
  size_t total = 0;
  for (size_t i = 0; i < NumPlanes(format); ++i)
    total += PlaneSize(format, i, size).getArea();
  return total;
}

size_t VideoFrame::NumPlanes(VideoPixelFormat format) {
  switch (format) {
    case VideoPixelFormat::UYVY:
    case VideoPixelFormat::ARGB:
      return 1;
    case VideoPixelFormat::UNKNOWN:
      break;
  }
  // todo: oh oh, unsupported pixel format
  return 0;
}

Size VideoFrame::PlaneSize(VideoPixelFormat format, size_t plane, const Size& codedSize)
{
  int width = codedSize.getWidth();
  int height = codedSize.getHeight();
  if (RequiresEvenSizeAllocation(format)) {
    // Align to multiple-of-two size overall. This ensures that non-subsampled
    // planes can be addressed by pixel with the same scaling as the subsampled
    // planes.
    width = s3d::math::roundUp(width, 2);
    height = s3d::math::roundUp(height, 2);
  }
  const Size subsample = SampleSize(format, plane);
  return Size(BytesPerElement(format, plane) * width / subsample.getWidth(),
                   height / subsample.getHeight());
}

bool VideoFrame::RequiresEvenSizeAllocation(VideoPixelFormat format)
{
  switch(format) {
    case VideoPixelFormat::UYVY:
    case VideoPixelFormat::ARGB:
      return false;
    case VideoPixelFormat::UNKNOWN:
      break;
  }

  // todo: oh oh
  return false;
}

Size VideoFrame::SampleSize(VideoPixelFormat format, size_t plane)
{
//  switch(plane) {
//    case kYPlane: // and kARGBPlane:
//    case kAPlane:
//      return Size(1, 1);
//    case kUPlane: // and kUVPlane:
//    case kVPlane:
//      switch (format) {
//        case VideoPixelFormat::UYVY:
//        case VideoPixelFormat::ARGB:
//          break;
//      }
//  }
  return {};
}

int VideoFrame::BytesPerElement(VideoPixelFormat format, size_t plane) {
  switch (format) {
    case VideoPixelFormat::ARGB:
      return 4;
    case VideoPixelFormat::UYVY:
      return 2;
    case VideoPixelFormat::UNKNOWN:
      break;
  }
  return 0;
}