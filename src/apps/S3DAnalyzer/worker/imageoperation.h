#ifndef WORKER_IMAGEOPERATION_H
#define WORKER_IMAGEOPERATION_H

#include "utilities/cv.h"

#include <gsl/gsl>

namespace cv {
  class Mat;
}

class ImageOperation {
public:
  bool applyOnImagesIfEnabled(cv::Mat* leftImage, cv::Mat* rightImage) {
    if (isEnabled) {
      applyOnImage(leftImage, rightImage);
    }
    return true;
  }

  void enable() { isEnabled = true; }
  void disable() { isEnabled = false; }

private:
  virtual bool applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage) = 0;

  bool isEnabled{true};
};

#include <s3d/cv/disparity/disparity_analyzer_stan.h>

class ImageOperationComputeAlignment : public ImageOperation {
public:
  explicit ImageOperationComputeAlignment(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer)
    : disparityAnalyzer_{disparityAnalyzer} {}

private:
  bool applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage) override {
    bool enoughFeaturePointsFound = disparityAnalyzer_->analyze(*leftImage, *rightImage);
    return enoughFeaturePointsFound; // && disparityRangeWideEnough();
  }

  bool disparityRangeWideEnough() {
    return (disparityAnalyzer_->results.maxDisparityPercent - disparityAnalyzer_->results.minDisparityPercent) > minDisparityRange_;
  }

  float minDisparityRange_{0.005};
  s3d::DisparityAnalyzerSTAN* disparityAnalyzer_;
};

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

#include <s3d/cv/rectification/rectifier_cv.h>

class ImageOperationRectify : public ImageOperation {
public:
  explicit ImageOperationRectify(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer)
          : disparityAnalyzer_{disparityAnalyzer} {}

private:
  bool applyOnImage(cv::Mat* leftImage, cv::Mat* rightImage) override {
    cv::Mat leftRect, rightRect;
    s3d::RectifierCV rectifier;
    auto alignment = disparityAnalyzer_->results.getStanAlignment();
    auto H1 = s3d::RectificationStan::leftImageMatrix(alignment);
    auto H2 = s3d::RectificationStan::rightImageMatrix(alignment);
    *leftImage = rectifier.rectifyCV(*leftImage, s3d::eigenMatToCV(H1));
    *rightImage = rectifier.rectifyCV(*rightImage, s3d::eigenMatToCV(H2));
    return true;
  }

  s3d::DisparityAnalyzerSTAN* disparityAnalyzer_;
};

class ImageOperationChain {
public:
  bool applyAllOperations(cv::Mat* leftImage, cv::Mat* rightImage) {
    for (auto* operation : operations) {
      if (!operation->applyOnImagesIfEnabled(leftImage, rightImage)) {
        return false;
      }
    }
    return true;
  }

  void setNextOperation(ImageOperation* operation) {
    operations.push_back(operation);
  }

private:
  std::vector<ImageOperation*> operations;
};

struct OperationEmptyImageException : public std::runtime_error {
  OperationEmptyImageException() : std::runtime_error("Camera Alignment Operation Chain on empty image") {}
};

class CameraAlignmentOperationChain {
public:
  explicit CameraAlignmentOperationChain(gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer)
    : disparityAnalyzer_{disparityAnalyzer}
    , computeAlignment_{disparityAnalyzer}
    , drawEpilines_{disparityAnalyzer}
    , rectify_{disparityAnalyzer}
  {
    // disabled by default
    drawEpilines_.disable();
    rectify_.disable();

    operationChain_.setNextOperation(&computeAlignment_);
    operationChain_.setNextOperation(&drawEpilines_);
    operationChain_.setNextOperation(&rectify_);
  }

  void setInputImages(const cv::Mat& leftImage, const cv::Mat& rightImage) {
    inputLeftImage = leftImage.clone();
    inputRightImage = rightImage.clone();
  }

  std::tuple<cv::Mat&, cv::Mat&> getOutputImages() {
    return { outputLeftImage, outputRightImage };
  };

  bool applyAllOperations() {
    outputLeftImage = inputLeftImage.clone();
    outputRightImage = inputRightImage.clone();
    if (outputLeftImage.empty() || outputRightImage.empty()) {
      return false;
    }
    return operationChain_.applyAllOperations(&outputLeftImage, &outputRightImage);
  }

  void enableAlignmentComputation() {  computeAlignment_.enable();  }
  void disableAlignmentComputation() {  computeAlignment_.disable();  }

  void enableDrawEpilines() {  drawEpilines_.enable();  }
  void disableDrawEpilines() {  drawEpilines_.disable();  }

  void enableRectify() {  rectify_.enable();  }
  void disableRectify() {  rectify_.disable();  }

private:
  bool isAnyImageEmpty() {
    return isImageEmpty(inputLeftImage) || isImageEmpty(inputRightImage);
  }

  bool isImageEmpty(const cv::Mat& image) {
    return image.rows == 0 || image.cols == 0;
  }

  cv::Mat inputLeftImage{};
  cv::Mat inputRightImage{};
  cv::Mat outputLeftImage{};
  cv::Mat outputRightImage{};

  s3d::DisparityAnalyzerSTAN* disparityAnalyzer_;
  ImageOperationComputeAlignment computeAlignment_;
  ImageOperationDrawEpilines drawEpilines_;
  ImageOperationRectify rectify_;
  ImageOperationChain operationChain_{};
};


#endif //WORKER_IMAGEOPERATION_H
