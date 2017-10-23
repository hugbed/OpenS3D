#ifndef S3DCV_DEMO_DATASETLOADER_H
#define S3DCV_DEMO_DATASETLOADER_H

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <Eigen/Dense>

#include <fstream>
#include <string>

/**
 * Utility class to load uncalibrated camera images dataset from:
 *
 *   J. Mallon and P.F. Whelan (2005),
 *   Projective rectification from the fundamental matrix,
 *   Image and Vision Computing,
 *   Volume 23, Issue 7 , 1 July 2005, Pages 643-650
 *
 * Example of a dataset directory structure
 *
 * dataset_path
 *   --> Arch
 *      --> Arch_L.bmp      // left image
 *      --> Arch_R.bmp      // right image
 *      --> Arch_L_H1.bmp   // left image rectified
 *      --> Arch_R_H2.bmp   // right image rectified
 *      --> Arch_F.txt      // fundamental matrix
 *      --> Arch_H1_L.txt   // left rectification matrix
 *      --> Arch_H2_R.txt   // right rectification matrix
 *      --> Arch_L.txt      // feature points left
 *      --> Arch_R.txt      // feature points right
 *   --> Drive
 *      --> ...
 *
 */
class DatasetLoaderVSG {
 public:
  explicit DatasetLoaderVSG(std::string name, std::string path)
      : name_{std::move(name)}, path_{std::move(path)} {}

  cv::Mat loadImageLeft() { return loadImageGrayscale(datasetPath() + "/" + name_ + "_L.bmp"); }

  cv::Mat loadImageRight() { return loadImageGrayscale(datasetPath() + "/" + name_ + "_R.bmp"); }

  cv::Mat loadImageLeftRectified() {
    return loadImageGrayscale(datasetPath() + "/" + name_ + "_L_H1.bmp");
  }

  cv::Mat loadImageRightRectified() {
    return loadImageGrayscale(datasetPath() + "/" + name_ + "_R_H2.bmp");
  }

  Eigen::Matrix3f loadFundamentalMatrix() {
    // don't know why I have to transpose here
    return loadMat3x3(datasetPath() + "/" + name_ + "_F.txt").transpose();
  }

  Eigen::Matrix3f loadRectificationMatrixLeft() {
    return loadMat3x3(datasetPath() + "/" + name_ + "_H1_L.txt");
  }

  Eigen::Matrix3f loadRectificationMatrixRight() {
    return loadMat3x3(datasetPath() + "/" + name_ + "_H2_R.txt");
  }

  std::vector<Eigen::Vector2d> loadPointsLeft() {
    return loadPoints(datasetPath() + "/" + name_ + "_L.txt");
  }

  std::vector<Eigen::Vector2d> loadPointsRight() {
    return loadPoints(datasetPath() + "/" + name_ + "_R.txt");
  }

 private:
  std::string datasetPath() { return path_ + "/" + name_; }

  cv::Mat loadImageGrayscale(const std::string& path) {
    cv::Mat image = cv::imread(path);
    cv::Mat imageGray;
    cv::cvtColor(image, imageGray, CV_BGR2GRAY);
    return imageGray;
  }

  /**
   * Loads a matrix from a file.
   * Columns are separated by spaces, rows by new lines.
   *
   * E.g:
   *
   * 1.12 -0.14 54.99
   * 0.20 1.02 -75.83
   * 0.00 0.00 1.00
   *
   */
  Eigen::Matrix3f loadMat3x3(const std::string& path) {
    Eigen::Matrix3f M;
    std::ifstream inputFile{path};
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; j++) {
        inputFile >> M(i, j);
      }
    }
    return M;
  }

  /**
   * Loads a set of points from a file.
   * Point coordinates are separated by spaces, points by new lines.
   *
   * E.g:
   *
   * 604.70 335.63
   * 618.92 333.84
   * 623.46 353.75
   *
   */
  std::vector<Eigen::Vector2d> loadPoints(const std::string& path) {
    std::vector<Eigen::Vector2d> points;

    std::ifstream inputFile{path};
    float x, y, z;
    while (inputFile >> x) {
      inputFile >> y;
      points.emplace_back(x, y);
    }
    return points;
  }

  std::string name_;
  std::string path_;
};

#endif  // S3DCV_DEMO_DATASETLOADER_H
