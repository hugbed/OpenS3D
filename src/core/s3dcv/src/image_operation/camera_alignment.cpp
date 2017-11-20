#include "s3d/cv/image_operation/camera_alignment.h"

namespace s3d {
namespace image_operation {

CameraAlignment::CameraAlignment(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer)
        : computeAlignment{disparityAnalyzer}
        , drawEpilines(disparityAnalyzer)
        , rectify(disparityAnalyzer)
        , inputOutputAdapter{&operations_} {
  operations_.setNext(&computeAlignment);
  operations_.setNext(&drawEpilines);
  operations_.setNext(&rectify);
}

} // namespace s3d
} // namespace image_operation
