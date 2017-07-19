#ifndef S3D_DISPARITY_VIEWER_CONTEXT_H
#define S3D_DISPARITY_VIEWER_CONTEXT_H

struct ViewerContext {
  ViewerContext(float viewerDistance,
                float interocularDistance,
                float screenWidth,
                int imageWidthPixels)
      : viewerDistance{viewerDistance},
        interocularDistance{interocularDistance},
        screenWidth{screenWidth},
        imageWidthPixels{imageWidthPixels} {}

  float viewerDistance;
  float interocularDistance;
  float screenWidth;
  int imageWidthPixels;
};

#endif  // S3D_DISPARITY_VIEWER_CONTEXT_H
