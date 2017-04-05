//
// Created by jon on 06/03/17.
//

#ifndef OPENCVTESTS_DISPARITYALGORITHMORB_H
#define OPENCVTESTS_DISPARITYALGORITHMORB_H

#include "DisparityAlgorithm.h"
#include "s3d/utilities/time.h"

#include <iostream>
#include <memory>

namespace s3d {

class DisparityAlgorithmORB : public DisparityAlgorithm {
public:
    std::unique_ptr<Disparities> ComputeDisparities(Image leftImg, Image rightImg) override;
};

}

#endif //OPENCVTESTS_DISPARITYALGORITHMORB_H
