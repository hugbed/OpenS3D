#include "s3d/cv/image_operation/draw_epilines.h"

#include "s3d/cv/disparity/disparity_analyzer_stan.h"
#include "s3d/cv/utilities/cv.h"

namespace s3d {
namespace image_operation {

DrawEpilines::DrawEpilines(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer)
  : disparityAnalyzer_{disparityAnalyzer} {}

bool DrawEpilines::applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage) {
  if (leftImage->empty() || rightImage->empty()) {
    return false;
  }

  std::tie(*leftImage, *rightImage) =
          s3d::drawEpipolarLines(s3d::eigenMatToCV(getFundamentalMatrix()),
                                 *leftImage,
                                 *rightImage,
                                 s3d::eigenPointsToCV(disparityAnalyzer_->results.featurePointsLeft),
                                 s3d::eigenPointsToCV(disparityAnalyzer_->results.featurePointsRight));
  return true;
}

Eigen::Matrix3d DrawEpilines::getFundamentalMatrix() {
  return s3d::StanFundamentalMatrixSolver::FundamentalMatrixFromAlignment(
          disparityAnalyzer_->results.getStanAlignment()
  );
}

} // namespace s3d
} // namespace image_operation