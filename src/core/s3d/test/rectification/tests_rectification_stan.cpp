#include "gtest/gtest.h"

#include "s3d/rectification/rectification_stan.h"

using s3d::RectificationStan;

// vertical offset offsets in pixels
// roll angle rotates only
// zoom unzooms by X % (scale is coherent)
// tilt offset ?
// panKeystone: regression testing
// tiltKeystone: regression testing
// zParallax deformation: regression testing. (though it may be hardcoded to 0 in rectification_stan)

TEST(rectification_stan, what_to_do_here) {
  EXPECT_TRUE(false);
}