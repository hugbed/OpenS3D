#ifndef WORKER_CAMERA_ALIGNMENT_OPERATION_CHAIN_H
#define WORKER_CAMERA_ALIGNMENT_OPERATION_CHAIN_H

#include "image_operation_compute_alignment.h"
#include "image_operation_draw_epilines.h"
#include "image_operation_rectify.h"
#include "image_operation_input_output_adapter.h"
#include "image_operations.h"


class CameraAlignmentOperations {
public:
  explicit CameraAlignmentOperations(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer);

  ImageOperationComputeAlignment computeAlignment;
  ImageOperationDrawEpilines drawEpilines;
  ImageOperationRectify rectify;
  ImageOperationsInputOutputAdapter inputOutputAdapter;

private:
  ImageOperationSequence operations_;
};

#endif // WORKER_CAMERA_ALIGNMENT_OPERATION_CHAIN_H
