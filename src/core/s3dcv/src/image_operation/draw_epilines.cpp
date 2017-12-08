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

// optionaly display matches
//  s3d::displayMatches("Inliers",
//                      *leftImage,
//                      *rightImage,
//                      disparityAnalyzer_->results.featurePointsLeft,
//                      disparityAnalyzer_->results.featurePointsRight);

  auto F = getFundamentalMatrix({leftImage->cols, leftImage->rows});

  std::tie(*leftImage, *rightImage) =
          s3d::drawEpipolarLines(s3d::eigenMatToCV(F),
                                 *leftImage,
                                 *rightImage,
                                 s3d::eigenPointsToCV(disparityAnalyzer_->results.featurePointsLeft),
                                 s3d::eigenPointsToCV(disparityAnalyzer_->results.featurePointsRight));
  return true;
}

Eigen::Matrix3d DrawEpilines::getFundamentalMatrix(const Size& imageSize) {
  return s3d::StanFundamentalMatrixSolver::CenteredFundamentalMatrixFromAlignment(
    disparityAnalyzer_->results.getStanAlignment(), imageSize
  );
}

} // namespace s3d
} // namespace image_operation