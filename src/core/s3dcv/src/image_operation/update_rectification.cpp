#include "s3d/cv/image_operation/update_rectification.h"

#include "s3d/multiview/stan_results.h"
#include "s3d/rectification/rectification_stan.h"

#include "s3d/cv/disparity/disparity_analyzer_stan.h"
#include "s3d/cv/utilities/cv.h"

namespace s3d {
namespace image_operation {

UpdateRectification::UpdateRectification(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer)
        : disparityAnalyzer_{disparityAnalyzer} {}

bool UpdateRectification::applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage, StanResults* results) {
  auto alignment = results->alignment;
  auto H1 = s3d::RectificationStan::centeredLeftImageMatrix(alignment, {leftImage->cols, leftImage->rows});
  auto H2 = s3d::RectificationStan::centeredRightImageMatrix(alignment, {leftImage->cols, leftImage->rows});

  results->HLeft = H1;
  results->HRight = H2;

  return true;
}

} // namespace s3d
} // namespace image_operation
