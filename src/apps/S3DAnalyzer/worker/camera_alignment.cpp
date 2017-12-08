#include "camera_alignment_operations.h"

CameraAlignmentOperations::CameraAlignmentOperations(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer)
    : computeAlignment{disparityAnalyzer},
      drawEpilines(disparityAnalyzer),
      rectify(disparityAnalyzer),
      inputOutputAdapter{&operations_} {
  operations_.setNext(&computeAlignment);
  operations_.setNext(&drawEpilines);
  operations_.setNext(&rectify);
}
