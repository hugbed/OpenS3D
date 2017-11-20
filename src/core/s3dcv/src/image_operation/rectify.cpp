#include "s3d/cv/image_operation/rectify.h"

#include "s3d/cv/disparity/disparity_analyzer_stan.h"
#include "s3d/cv/rectification/rectifier_cv.h"
#include "s3d/cv/utilities/cv.h"

namespace s3d {
namespace image_operation {

Rectify::Rectify(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer)
  : disparityAnalyzer_{disparityAnalyzer} {}

bool Rectify::applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage) {
  cv::Mat leftRect, rightRect;
  s3d::RectifierCV rectifier;
  auto alignment = disparityAnalyzer_->results.getStanAlignment();
  auto H1 = s3d::RectificationStan::leftImageMatrix(alignment);
  auto H2 = s3d::RectificationStan::rightImageMatrix(alignment);
  *leftImage = rectifier.rectifyCV(*leftImage, s3d::eigenMatToCV(H1));
  *rightImage = rectifier.rectifyCV(*rightImage, s3d::eigenMatToCV(H2));
  return true;
}

} // namespace s3d
} // namespace image_operation