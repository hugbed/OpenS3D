#include "s3d/cv/image_operation/camera_alignment.h"

namespace s3d {
namespace image_operation {

CameraAlignment::CameraAlignment(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer)
        : scaleImages{}
        , computeAlignment{disparityAnalyzer}
        , filterAlignment{disparityAnalyzer}
        , drawEpilines(disparityAnalyzer)
        , updateRectification(disparityAnalyzer)
        , rectify(disparityAnalyzer)
        , inputOutputAdapter{&operations_} {
  operations_.setNext(&scaleImages);
  operations_.setNext(&computeAlignment);
  operations_.setNext(&filterAlignment);
  operations_.setNext(&drawEpilines);
  operations_.setNext(&updateRectification);
  operations_.setNext(&rectify);

  // disabled by default
  updateRectification.disable();
}

} // namespace s3d
} // namespace image_operation
