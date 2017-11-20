#ifndef WORKER_IMAGEOPERATIONDRAWEPILINES_H_H
#define WORKER_IMAGEOPERATIONDRAWEPILINES_H_H

#include "image_operation.h"

#include <s3d/cv/disparity/disparity_analyzer_stan.h>
#include <s3d/cv/utilities/cv.h>

class ImageOperationDrawEpilines : public ImageOperation {
public:
  explicit ImageOperationDrawEpilines(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer)
          : disparityAnalyzer_{disparityAnalyzer} {}

private:
  bool applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage) override {
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

  Eigen::Matrix3d getFundamentalMatrix() {
    return s3d::StanFundamentalMatrixSolver::FundamentalMatrixFromAlignment(
            disparityAnalyzer_->results.getStanAlignment()
    );
  }

  s3d::DisparityAnalyzerSTAN* disparityAnalyzer_;
};

#endif // WORKER_IMAGEOPERATIONDRAWEPILINES_H_H
