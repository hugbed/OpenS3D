#ifndef WORKER_CAMERA_ALIGNMENT_OPERATION_CHAIN_H
#define WORKER_CAMERA_ALIGNMENT_OPERATION_CHAIN_H

#include "s3d/cv/image_operation/compute_alignment.h"
#include "s3d/cv/image_operation/draw_epilines.h"
#include "s3d/cv/image_operation/rectify.h"
#include "s3d/cv/image_operation/input_output_adapter.h"
#include "s3d/cv/image_operation/image_operations.h"

class CameraAlignmentOperations {
public:
  explicit CameraAlignmentOperations(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer);

  s3d::image_operation::ComputeAlignment computeAlignment;
  s3d::image_operation::DrawEpilines drawEpilines;
  s3d::image_operation::Rectify rectify;
  s3d::image_operation::InputOutputAdapter inputOutputAdapter;

private:
  s3d::image_operation::Sequence operations_;
};

#endif // WORKER_CAMERA_ALIGNMENT_OPERATION_CHAIN_H
