#ifndef S3D_CV_UTILITIES_CV_H
#define S3D_CV_UTILITIES_CV_H

#include "s3d/image/image.h"
#include "s3d/utilities/random_color_generator.h"
#include "s3d/video/video_frame.h"
#include "s3d/video/video_types.h"

#include "Eigen/Dense"

#include <opencv/cxeigen.hpp>
#include <opencv2/opencv.hpp>

#include <cstdint>

#include <gsl/gsl>

#include <cassert>
#include <cmath>
#include <tuple>

namespace s3d {

inline Image<uint8_t> cv2image(cv::Mat cvImg) {
  Image<uint8_t> img(Size(cvImg.cols, cvImg.rows));
  cv::cv2eigen(cvImg, img);
  return img;
}

inline cv::Mat image2cv(const Image<uint8_t>& img) {
  cv::Mat cvImg;
  cv::eigen2cv(img, cvImg);
  return cvImg;
}

inline int nbChannelsToMatType(int nbChannels) {
  int imgType = CV_8U;
  switch (nbChannels) {
    case 1:
      imgType = CV_8U;
      break;
    case 2:
      imgType = CV_8UC2;
      break;
    case 3:
      imgType = CV_8UC3;
      break;
    case 4:
      imgType = CV_8UC4;
      break;
    default:
      break;
  }
  return imgType;
}

inline cv::Mat dataToMat(Size size, VideoPixelFormat pixelFormat, gsl::span<const uint8_t> data) {
  auto nbChannels = static_cast<int>(VideoFrame::NumBytesPerPixel(pixelFormat));
  return cv::Mat(size.getHeight(),
                 size.getWidth(),
                 nbChannelsToMatType(nbChannels),
                 const_cast<uchar*>(data.data()));
}
// does not copy data, to copy use resizeMat(...).clone();
inline void resizeMat(gsl::not_null<cv::Mat*> mat, float colsRatio, float rowsRatio) {
  float cols = mat->cols;
  float rows = mat->rows;
  cv::resize(*mat, *mat, cv::Size2f(cols * colsRatio, rows * rowsRatio), cv::INTER_AREA);
}

inline void resizeMat(gsl::not_null<cv::Mat*> mat, float ratio) {
  resizeMat(mat, ratio, ratio);
}

inline void displayInNewWindow(const std::string& name, cv::InputArray src) {
  cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
  cv::imshow(name, src);
}

template <class T>
void displayMatches(const std::string& title,
                    const cv::Mat& left,
                    const cv::Mat& right,
                    const std::vector<Eigen::Matrix<T, 2, 1>>& featuresLeft,
                    const std::vector<Eigen::Matrix<T, 2, 1>>& featuresRight) {
  cv::Mat leftRight(left.rows, left.cols * 2, left.type());
  cv::hconcat(left, right, leftRight);

  // convert to color
  cv::cvtColor(leftRight, leftRight, cv::COLOR_GRAY2BGR);

  // for random color
  RandomColorGenerator<cv::Scalar> colorGenerator;

  for (int i = 0; i < featuresLeft.size(); ++i) {
    // random color
    cv::Scalar color = colorGenerator.randomColor();

    // discretize features
    const Eigen::Vector2i featureLeft = featuresLeft[i].template cast<int>();
    const Eigen::Vector2i featureRight =
        featuresRight[i].template cast<int>() + Eigen::Vector2i(left.cols, 0.0f);

    const cv::Point ptLeft(featureLeft.x(), featureLeft.y());
    const cv::Point ptRight(featureRight.x(), featureRight.y());

    cv::line(leftRight, ptLeft, ptRight, color, 1, CV_AA);
    cv::circle(leftRight, ptLeft, 3, color, 1, CV_AA);
    cv::circle(leftRight, ptRight, 3, color, 1, CV_AA);
  }

  displayInNewWindow(title, leftRight);
}

template <class T>
std::vector<cv::Point_<T>> eigenPointsToCV(std::vector<Eigen::Matrix<T, 2, 1>> points) {
  std::vector<cv::Point_<T>> pointsCV;
  for (int i = 0; i < points.size(); ++i) {
    auto& ptLeft = points[i];
    pointsCV.emplace_back(ptLeft.x(), ptLeft.y());
  }
  return pointsCV;
}

template <class T, int N, int M>
cv::Matx<T, N, M> eigenMatToCV(Eigen::Matrix<T, N, M> mat) {
  cv::Matx<T, 3, 3> matCV;
  cv::eigen2cv(mat, matCV);
  return matCV;
};

template <typename T>
static float distancePointLine(const cv::Point_<T> point, const cv::Vec<T, 3>& line) {
  // Line is given as a*x + b*y + c = 0
  return std::abs(line(0) * point.x + line(1) * point.y + line(2)) /
         std::sqrt(line(0) * line(0) + line(1) * line(1));
}

/**
 * \brief Compute and draw the epipolar lines in two images
 *      associated to each other by a fundamental matrix
 *
 * \param title     Title of the window to display
 * \param F         Fundamental matrix
 * \param img1      First image
 * \param img2      Second image
 * \param points1   Set of points in the first image
 * \param points2   Set of points in the second image matching to the first set
 * \param inlierDistance      Points with a high distance to the epipolar lines are
 *                not displayed. If it is negative, all points are displayed
 **/
template <typename T1, typename T2>
static std::tuple<cv::Mat, cv::Mat> drawEpipolarLines(const cv::Matx<T1, 3, 3> F,
                                                      const cv::Mat& img1,
                                                      const cv::Mat& img2,
                                                      const std::vector<cv::Point_<T2>> points1,
                                                      const std::vector<cv::Point_<T2>> points2,
                                                      const float inlierDistance = -1) {
  assert(img1.size() == img2.size() && img1.type() == img2.type());
  assert(points1.size() == points2.size());
  assert(points1.size() > 0);

  cv::Mat outImgLeft(img1.rows, img1.cols, CV_8UC3);
  cv::Mat outImgRight(img1.rows, img1.cols, CV_8UC3);

  // for color drawing
  if (img1.type() == CV_8U) {
    cv::cvtColor(img1, outImgLeft, CV_GRAY2BGR);
    cv::cvtColor(img2, outImgRight, CV_GRAY2BGR);
  } else {
    img1.copyTo(outImgLeft);
    img2.copyTo(outImgRight);
  }

  // compute epilines for image 1 and 2
  std::vector<cv::Vec<T2, 3>> epilines1, epilines2;
  cv::computeCorrespondEpilines(points1, 1, F, epilines1);
  cv::computeCorrespondEpilines(points2, 2, F, epilines2);

  assert(points1.size() == points2.size() && points2.size() == epilines1.size() &&
         epilines1.size() == epilines2.size());

  s3d::RandomColorGenerator<cv::Scalar> colorGenerator;
  for (size_t i = 0; i < points1.size(); i++) {
    if (inlierDistance > 0) {
      if (distancePointLine(points1[i], epilines2[i]) > inlierDistance ||
          distancePointLine(points2[i], epilines1[i]) > inlierDistance) {
        // The point match is no inlier
        continue;
      }
    }

    // epipolar lines of the 1st point set are drawn in the 2nd image and vice-versa
    cv::Scalar color = colorGenerator.randomColor();

    cv::line(
        outImgRight,
        cv::Point(0, -epilines1[i][2] / epilines1[i][1]),
        cv::Point(img1.cols, -(epilines1[i][2] + epilines1[i][0] * img1.cols) / epilines1[i][1]),
        color,
        1,
        CV_AA);
    cv::circle(outImgLeft, points1[i], 3, color, -1, CV_AA);

    cv::line(
        outImgLeft,
        cv::Point(0, -epilines2[i][2] / epilines2[i][1]),
        cv::Point(img2.cols, -(epilines2[i][2] + epilines2[i][0] * img2.cols) / epilines2[i][1]),
        color,
        1,
        CV_AA);
    cv::circle(outImgRight, points2[i], 3, color, -1, CV_AA);
  }

  return std::tuple<cv::Mat, cv::Mat>(outImgLeft, outImgRight);
}

/**
 * \brief Compute and draw the epipolar lines in two images
 *      associated to each other by a fundamental matrix
 *
 * \param title     Title of the window to display
 * \param F         Fundamental matrix
 * \param img1      First image
 * \param img2      Second image
 * \param points1   Set of points in the first image
 * \param points2   Set of points in the second image matching to the first set
 * \param inlierDistance      Points with a high distance to the epipolar lines are
 *                not displayed. If it is negative, all points are displayed
 **/
template <typename T1, typename T2>
static void displayEpipolarLines(const std::string& title,
                                 const cv::Matx<T1, 3, 3> F,
                                 const cv::Mat& img1,
                                 const cv::Mat& img2,
                                 const std::vector<cv::Point_<T2>> points1,
                                 const std::vector<cv::Point_<T2>> points2,
                                 const float inlierDistance = -1) {
  assert(img1.size() == img2.size() && img1.type() == img2.type());

  cv::Mat imgLeftEpilines, imgRightEpilines;
  std::tie(imgLeftEpilines, imgRightEpilines) =
      s3d::drawEpipolarLines(F, img1, img2, points1, points2);

  // horizontal concat
  cv::Mat outImg(img1.rows, img1.cols * 2, CV_8UC3);
  cv::hconcat(imgLeftEpilines, imgRightEpilines, outImg);

  cv::imshow(title, outImg);
  cv::waitKey(1);
}

}  // namespace s3d

#endif  // S3D_CV_UTILITIES_CV_H
