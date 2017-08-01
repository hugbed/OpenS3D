#ifndef UTILITIES_USERSETTINGS_H
#define UTILITIES_USERSETTINGS_H

#include <s3d/disparity/viewer_context.h>
#include <s3d/disparity/viewer_depth_converter.h>

struct DisplayParameters {
  float displayRangeMin{-5.0f};
  float displayRangeMax{5.0f};
  float expectedRangeMin{-1.1f};
  float expectedRangeMax{3.1f};
};

struct UserSettings {
  s3d::ViewerContext viewerContext{2.0, 0.06f, 1.0f, 1920};
  DisplayParameters displayParameters;
  float viewerDisplayZoom{1.0f};
};

#endif  // UTILITIES_USERSETTINGS_H
