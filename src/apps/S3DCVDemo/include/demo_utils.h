#ifndef S3DCV_DEMO_DEMO_UTILS_H
#define S3DCV_DEMO_DEMO_UTILS_H

#include <s3d/cv/disparity/disparity_analyzer_stan.h>

#include <gsl/gsl>

class BadNumberOfArgumentsException {};
class FileNotFoundException {};

/**
 * Creates dataset loader from program input arguments.
 *
 * First input argument must be the dataset folder, e.g. 'Arch'
 * Second input argument must be the dataset path e.g. '/home/you/dataset'
 *
 */
inline DatasetLoaderVSG createDatasetLoader(int argc, char* argv[]) {
  auto input_args = gsl::span<char*>(argv, argc);
  if (input_args.size() != 3) {
    throw BadNumberOfArgumentsException{};
  }
  const std::string datasetName = input_args[1];
  const std::string datasetPath = input_args[2];

  return DatasetLoaderVSG{datasetName, datasetPath};
}

inline s3d::RansacAlgorithm<s3d::StanFundamentalMatrixSolver, s3d::SampsonDistanceFunction> createRansac(
        s3d::Size imageSize) {
  int h = imageSize.getHeight();
  int w = imageSize.getWidth();

  s3d::Ransac::Params params;
  params.nbTrials = 500;
  params.distanceThreshold = 0.01 * sqrt(h * h + w * w);

  return s3d::DisparityAnalyzerSTAN::RansacAlgorithmSTAN(params);
}

#endif //S3DCV_DEMO_DEMO_UTILS_H
