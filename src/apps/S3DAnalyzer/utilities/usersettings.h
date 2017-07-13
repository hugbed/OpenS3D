#ifndef PROJECT_USERSETTINGS_H
#define PROJECT_USERSETTINGS_H

#include <s3d/disparity/viewer_depth_converter.h>

struct DisplayParameters {
  float displayRangeMin{-5.0f};
  float displayRangeMax{5.0f};
  float expectedRangeMin{-1.1f};
  float expectedRangeMax{3.1f};
};

struct UserSettings {
  ViewerContext viewerContext{2.0, 0.06f, 1.0f, 1920};
  DisplayParameters displayParameters;
};

#endif  // PROJECT_USERSETTINGS_H
