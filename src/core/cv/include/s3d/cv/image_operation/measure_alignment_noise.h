#ifndef S3D_CV_IMAGE_OPERATION_MESURE_ALIGNMENT_NOISE_H
#define S3D_CV_IMAGE_OPERATION_MESURE_ALIGNMENT_NOISE_H

#include "image_operation.h"

#include <s3d/filter/stan_variance.h>
#include <s3d/utilities/stats.h>

#include <vector>
#include <s3d/multiview/stan_results.h>

namespace cv {
class Mat;
}

namespace s3d {
namespace image_operation {

class MeasureAlignmentNoise : public ImageOperation {
public:
  StanVariance getVariance() {
    StanVariance variance;
    variance.vertical = vertical.getVariance();
    variance.roll = roll.getVariance();
    variance.zoom = zoom.getVariance();
    variance.tiltOffset = tiltOffset.getVariance();
    variance.panKeystone = panKeystone.getVariance();
    variance.tiltKeystone = tiltKeystone.getVariance();
//    variance.zParallaxDeformation = zParallaxDeformation.getVariance();

    return variance;
  }

  void reset() {
    vertical.reset();
    roll.reset();
    zoom.reset();
    tiltOffset.reset();
    panKeystone.reset();
    tiltKeystone.reset();
    zParallaxDeformation.reset();
  }

private:
  bool applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage, StanResults* results) override {
    vertical.update(results->alignment.verticalOffsetRadians());
    roll.update(results->alignment.rollAngleRadians());
    zoom.update(results->alignment.zoomRatio());
    tiltOffset.update(results->alignment.tiltOffsetPixels());
    panKeystone.update(results->alignment.panKeystoneRadiansPerMeter());
    tiltKeystone.update(results->alignment.tiltKeystoneRadiansPerMeter());
    zParallaxDeformation.update(results->alignment.zParallaxDeformationRatio());
  }

  OnlineVariance<double> vertical;
  OnlineVariance<double> roll;
  OnlineVariance<double> zoom;
  OnlineVariance<double> tiltOffset;
  OnlineVariance<double> panKeystone;
  OnlineVariance<double> tiltKeystone;
  OnlineVariance<double> zParallaxDeformation;
};

} // namespace image_operation
} // namespace s3d

#endif //S3D_CV_IMAGE_OPERATION_MESURE_ALIGNMENT_NOISE_H
