#ifndef S3D_CV_IMAGE_OPERATION_CAMERA_ALIGNMENT_H
#define S3D_CV_IMAGE_OPERATION_CAMERA_ALIGNMENT_H

#include "s3d/cv/image_operation/scale_images.h"
#include "s3d/cv/image_operation/compute_alignment.h"
#include "s3d/cv/image_operation/filter_alignment.h"
#include "s3d/cv/image_operation/draw_epilines.h"
#include "s3d/cv/image_operation/update_rectification.h"
#include "s3d/cv/image_operation/rectify.h"
#include "s3d/cv/image_operation/input_output_adapter.h"
#include "s3d/cv/image_operation/image_operations.h"

namespace s3d {
  class DisparityAnalyzerSTAN;
}

namespace s3d {
namespace image_operation {

class CameraAlignment {
public:
  explicit CameraAlignment(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer);

  ScaleImages scaleImages;
  ComputeAlignment computeAlignment;
  FilterAlignment filterAlignment;
  DrawEpilines drawEpilines;
  UpdateRectification updateRectification;
  Rectify rectify;
  InputOutputAdapter inputOutputAdapter;

private:
  Sequence operations_;
};

} // namespace s3d
} // namespace image_operation

#endif // S3D_CV_IMAGE_OPERATION_CAMERA_ALIGNMENT_H
