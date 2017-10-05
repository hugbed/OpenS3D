#ifndef S3D_DISPARITY_VIEWER_CONTEXT_H
#define S3D_DISPARITY_VIEWER_CONTEXT_H

namespace s3d {

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
}  // namespace s3d

#endif  // S3D_DISPARITY_VIEWER_CONTEXT_H
