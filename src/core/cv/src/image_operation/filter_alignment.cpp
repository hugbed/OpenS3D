#include "s3d/cv/image_operation/filter_alignment.h"

#include "s3d/cv/disparity/disparity_analyzer_stan.h"

#include <opencv2/core/mat.hpp>

namespace s3d {
namespace image_operation {

FilterAlignment::FilterAlignment(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer)
        : disparityAnalyzer_{disparityAnalyzer}
        , kalmanFilter_{5}
{
  Eigen::VectorXd observationVariances(5);
  observationVariances << 3.6816E-4, 5.9837E-7, 7.2080E-6, 0.7405, 3.6816E-4;
  kalmanFilter_.setObservationVariances(observationVariances);

  Eigen::VectorXd processVariances(5);
  processVariances << 0.0000001, 0.00001, 0.00001, 0.00001, 0.00001;
  kalmanFilter_.setProcessVariances(processVariances);
}

bool FilterAlignment::applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage, StanResults* results) {
  Eigen::VectorXd alignment(5);
  alignment << results->alignment.ch_y,
          results->alignment.a_z,
          results->alignment.a_f,
          results->alignment.f_a_x,
          results->alignment.a_y_f;

  kalmanFilter_.filter(alignment);

  Eigen::VectorXd filteredAlignment = kalmanFilter_.getEstimation();

  results->alignment.ch_y = filteredAlignment(0);
  results->alignment.a_z = filteredAlignment(1);
  results->alignment.a_f = filteredAlignment(2);
  results->alignment.f_a_x = filteredAlignment(3);
  results->alignment.a_y_f = filteredAlignment(4);

  return true;
}

void FilterAlignment::resetFilter() {
  kalmanFilter_.setInitialValues(Eigen::VectorXd::Zero(5));
}


} // namespace s3d
} // namespace image_operation