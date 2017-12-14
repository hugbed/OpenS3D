#include "s3d/cv/image_operation/rectify.h"

#include "s3d/cv/disparity/disparity_analyzer_stan.h"
#include "s3d/cv/rectification/rectifier_cv.h"
#include "s3d/cv/utilities/cv.h"

namespace s3d {
namespace image_operation {

Rectify::Rectify(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer)
  : disparityAnalyzer_{disparityAnalyzer} {}

bool Rectify::applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage, StanResults* results) {
  Eigen::Matrix3f H1 = results->HLeft;
  Eigen::Matrix3f H2 = results->HRight;

  s3d::RectifierCV rectifier;
  *leftImage = rectifier.rectifyCV(*leftImage, s3d::eigenMatToCV(H1));
  *rightImage = rectifier.rectifyCV(*rightImage, s3d::eigenMatToCV(H2));

  rectifyFeatures(results);

  return true;
}

// todo: do in GPU
void Rectify::rectifyFeatures(StanResults* results) {
  Eigen::Matrix3f H1 = results->HLeft;
  Eigen::Matrix3f H2 = results->HRight;

  results->featuresLeft.resize(disparityAnalyzer_->results.stan.featuresLeft.size());
  results->featuresRight.resize(disparityAnalyzer_->results.stan.featuresRight.size());
  for (int i = 0; i < disparityAnalyzer_->results.stan.featuresLeft.size(); ++i) {
    auto x = disparityAnalyzer_->results.stan.featuresLeft[i];
    auto xp = disparityAnalyzer_->results.stan.featuresRight[i];

    Eigen::Vector3f x_h(x.x(), x.y(), 1.0);
    Eigen::Vector3f xp_h(xp.x(), xp.y(), 1.0);

    x_h =  H1*x_h;
    xp_h = H2*xp_h;

    results->featuresLeft[i] = Eigen::Vector2f(x_h.x(), x_h.y());
    results->featuresRight[i] = Eigen::Vector2f(xp_h.x(), xp_h.y());
  }
}

} // namespace s3d
} // namespace image_operation