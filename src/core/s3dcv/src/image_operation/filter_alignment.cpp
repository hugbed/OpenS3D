#include "s3d/cv/image_operation/filter_alignment.h"

#include "s3d/cv/disparity/disparity_analyzer_stan.h"

#include <opencv2/core/mat.hpp>

namespace s3d {
namespace image_operation {

FilterAlignment::FilterAlignment(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer)
  : disparityAnalyzer_{disparityAnalyzer}
  , kalmanFilter_{4}
{
  Eigen::VectorXd variances(4);
  variances << 3.6816E-4, 5.9837E-7, 7.2080E-6, 0.7405;
  kalmanFilter_.setVariances(variances);
}

bool FilterAlignment::applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage, StanResults* results) {
  results->alignment;

  Eigen::VectorXd alignment(4);
  alignment << results->alignment.ch_y,
          results->alignment.a_z,
          results->alignment.a_f,
          results->alignment.f_a_x;

  kalmanFilter_.filter(alignment);

  Eigen::VectorXd filteredAligment = kalmanFilter_.getEstimation();

  results->alignment.ch_y = filteredAligment(0);
  results->alignment.a_z = filteredAligment(1);
  results->alignment.a_f = filteredAligment(2);
  results->alignment.f_a_x = filteredAligment(3);

  return true;
}


} // namespace s3d
} // namespace image_operation