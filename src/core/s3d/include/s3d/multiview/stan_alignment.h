#ifndef S3D_MULTIVIEW_STAN_ALIGNMENT_H
#define S3D_MULTIVIEW_STAN_ALIGNMENT_H

// todo: use math defines in .cpp file
#define _USE_MATH_DEFINES
#include <math.h>

namespace s3d {

// todo: variable names are really not clear (we assume here that one has read the article...)
// todo: name this with readable format (i.e: verticalOffset, etc.)
struct StanAlignment {
  // logical representations and units
  double verticalOffsetDegrees() { return ch_y * 180 / M_PI; }
  double rollAngleDegrees() { return a_z * 180.0 / M_PI; }
  double zoomRatioPercent() { return (a_f + 1.0) * 100.0; }
  double tiltOffsetPixels() { return f_a_x; }
  double panKeystoneDegreesPerMeter() { return a_y_f * 180 / M_PI; }
  double tiltKeystoneDegreesPerMeter() { return a_x_f * 180 / M_PI; }
  double zParallaxDeformationRatio() { return ch_z_f; }

  // mathematical symbols
  double ch_y{};    // vertical offset: * 180 / PI (degrees)
  double a_z{};     // roll angle: a_z * 180.0 / PI (degrees)
  double a_f{};     // zoom: (a_f + 1.0) * 100.0 (%)
  double f_a_x{};   // tiltOffset: pixels
  double a_y_f{};   // panKeystone: radians / m
  double a_x_f{};   // tiltKeystone: radians / m
  double ch_z_f{};  // zParallaxDeformation: ratio (m/m)
};

} // namespace s3d

#endif //S3D_MULTIVIEW_STAN_ALIGNMENT_H
