#include "s3d/robust_estimation/ransac.h"
#include "s3d/multiview/sampson_distance_function.h"
#include "s3d/multiview/stan_fundamental_matrix_solver.h"
#include "s3d/disparity/utilities.h"
#include <s3d/cv/disparity/disparity_analyzer_stan.h>
#include "s3d/cv/features/match_finder_cv.h"
#include "s3d/cv/utilities/cv.h"
#include "s3d/utilities/histogram.h"
#include "s3d/utilities/math.h"

#include <opencv2/opencv.hpp>

class BadNumberOfArgumentsException {};
class FileNotFoundException {};

class RandomColorGenerator {

public:
  RandomColorGenerator()
    : mt(std::random_device{}())
    , dist(0, 255)
  {
  }

  cv::Scalar randomColor() {
    return cv::Scalar(dist(mt), dist(mt), dist(mt));
  }

  std::mt19937 mt;
  std::uniform_int_distribution<int> dist;
};

RandomColorGenerator g_randColorGen{};

std::ostream& operator<<(std::ostream& out, const s3d::StanAlignment& model) {
  out << "Vertical offset (degrees) = " << model.ch_y * 3.141592653589793 / 180.0 << std::endl;
  out << "Roll angle (degrees) = " << model.a_z * 3.141592653589793 / 180.0 << std::endl;
  out << "Focal distance ratio (%) = " << (model.a_f + 1.0) * 100.0 << std::endl;
  out << "Tilt offset (pixels) = " << model.f_a_x << std::endl;
  out << "Pan keystone = " << model.a_x_f << std::endl;
  out << "Tilt offset keystone = " << model.a_y_f << std::endl;
  out << "Z parallax deformation =  " << model.ch_z_f << std::endl << std::endl;
  return out;
}

std::ostream& operator<<(std::ostream& out, const s3d::DisparityAnalyzerSTAN::Results& results) {
  out << "Vertical offset (degrees) = "
      << static_cast<double>(results.vertical) * 180.0 / 3.141592653589793 << std::endl;
  out << "Roll angle (degrees) = " << static_cast<double>(results.roll) * 180.0 / 3.141592653589793
      << std::endl;
  out << "Focal distance ratio (%) = " << static_cast<double>(results.zoom) << std::endl;
  out << "Tilt offset (pixels) = " << static_cast<double>(results.tiltOffset)
      << std::endl;  // pixels
  out << "Pan keystone = " << static_cast<double>(results.panKeystone) << std::endl;
  out << "Tilt offset keystone = " << static_cast<double>(results.tiltKeystone) << std::endl;
  out << "Z parallax deformation =  " << static_cast<double>(results.zParallaxDeformation)
      << std::endl
      << std::endl;
  return out;
}

void displayInNewWindow(const std::string& name, cv::InputArray src) {
  cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
  cv::imshow(name, src);
  cv::waitKey(0);
}

void drawFeatures(cv::Mat img, const std::vector<Eigen::Vector2f>& pts) {
  for (auto& pt : pts) {
    cv::circle(img, cv::Point(pt.x(), pt.y()), 2, cv::Scalar(255, 0, 0, 0), -1);
  }
}

cv::Mat fundamentalMatrixFromResults(const s3d::DisparityAnalyzerSTAN::Results& results) {
  auto alignment = results.getStanAlignment();
  auto ch_y = static_cast<float>(alignment.ch_y);
  auto a_z = static_cast<float>(alignment.a_z);
  auto a_f = static_cast<float>(alignment.a_f);
  auto f_a_x = static_cast<float>(alignment.f_a_x);
  auto a_x_f = static_cast<float>(alignment.a_x_f);
  auto a_y_f = static_cast<float>(alignment.a_y_f);    // convergence
  auto ch_z_f = static_cast<float>(alignment.ch_z_f);  // ch_z = 0.0

  std::vector<float> Fdata{
          0, (-ch_z_f + a_y_f), (ch_y + a_z), (ch_z_f), (-a_x_f), (-1 + a_f), (-ch_y), 1, (-f_a_x)};

  cv::Mat F(3, 3, CV_32F, Fdata.data());

  return F;
}

// draw epipolar lines
void drawEpipolarLines2(cv::Mat& image_out, // output image
                        const cv::Mat& image1, // image 1
                        const cv::Mat& image2, // image 2
                        std::vector<cv::Point2f>& points1, // keypoints 1
                        std::vector<cv::Point2f>& points2, // keypoints 2)
                        cv::Mat F,
                        int whichImage) // image to compute epipolar lines in
{
  std::vector<cv::Vec3f> lines1;

  // Compute corresponding epipolar lines
  cv::computeCorrespondEpilines(cv::Mat(points1), // image points
                                whichImage, // in image 1 (can also be 2)
                                F, // F matrix
                                lines1); // vector of epipolar lines

  // convert to color
  cv::cvtColor(image_out, image_out, cv::COLOR_GRAY2BGR);

  // for all epipolar lines
  int i = 0;
  for (std::vector<cv::Vec3f>::const_iterator it = lines1.begin(); it != lines1.end(); ++it, ++i) {
    // random color
    auto color = g_randColorGen.randomColor();

    // Draw the line between first and last column
    cv::line(image2, //
             cv::Point(0, -(*it)[2] / (*it)[1]), //
             cv::Point(image2.cols, -((*it)[2] + (*it)[0] * image2.cols) / (*it)[1]), //
             color);

    cv::circle(image2, cv::Point(points2[i].x, points2[i].y), 10, color);
  }
}


void resultsToKeypoints(const s3d::DisparityAnalyzerSTAN::Results& results,
                        gsl::not_null<std::vector<cv::Point2f>*>points1,
                        gsl::not_null<std::vector<cv::Point2f>*>points2) {
  // eigen to cv::Point2f
  points1->clear();
  points2->clear();
  for (int i = 0; i < results.featurePointsLeft.size(); ++i) {
    auto && point1 = results.featurePointsLeft[i];
    auto && point2 = results.featurePoints[i];
    points1->emplace_back(point1.x(), point1.y());
    points2->emplace_back(point2.x(), point2.y());
  }
}

void drawEpipolarLines(cv::Mat& image_out, // output image
                                              cv::Mat& image1, // image 1
                                              cv::Mat& image2, // image 2
                                              std::vector<cv::Point2f>& points1, // keypoints 1
                                              std::vector<cv::Point2f>& points2, // keypoints 2)
                                              int whichImage) // image to compute epipolar lines in
{
  // Compute F matrix from 7 matches
  cv::Mat F = cv::findFundamentalMat(cv::Mat(points1), // points in object image
                                     cv::Mat(points2), // points in scene image
                                     cv::FM_7POINT); // 7-point method

  std::vector<cv::Vec3f> lines1;

  // Compute corresponding epipolar lines
  cv::computeCorrespondEpilines(cv::Mat(points1), // image points
                                whichImage, // in image 1 (can also be 2)
                                F, // F matrix
                                lines1); // vector of epipolar lines

  // for all epipolar lines
  for (std::vector<cv::Vec3f>::const_iterator it = lines1.begin(); it!=lines1.end(); ++it)
  {
    // Draw the line between first and last column
    cv::line(image_out,
             cv::Point(0,-(*it)[2]/(*it)[1]),
             cv::Point(image2.cols,-((*it)[2]+
                                     (*it)[0]*image2.cols)/(*it)[1]),
             cv::Scalar(255,255,255));
  }

}

std::pair<cv::Mat, cv::Mat> rectifyImages(const cv::Mat& left,
                                          const cv::Mat& right,
                                          const s3d::StanAlignment& alignment) {
  auto ch_y = static_cast<float>(alignment.ch_y);
  auto a_z = static_cast<float>(alignment.a_z);
  auto a_f = static_cast<float>(alignment.a_f);
  auto f_a_x = static_cast<float>(alignment.f_a_x);
  auto a_x_f = static_cast<float>(alignment.a_x_f);
  auto a_y_f = static_cast<float>(alignment.a_y_f);    // convergence
  auto ch_z_f = static_cast<float>(alignment.ch_z_f);  // ch_z = 0.0

  // to rotate around center
  std::vector<float> Hoffdata{1.0f, 0.0f, static_cast<float>(left.cols / 2),
                              0.0f, 1.0f, static_cast<float>(left.rows / 2),
                              0.0f, 0.0f, 1.0f};

  std::vector<float> HoffTdata{1.0f, 0.0f, -static_cast<float>(left.cols / 2),
                               0.0f, 1.0f, -static_cast<float>(left.rows / 2),
                               0.0f, 0.0f, 1.0f};

  std::vector<float> Hdata{1.0f,    ch_y, 0.0f,  // f * ch_z = 0
                           -ch_y,   1,    0,     //
                           -ch_z_f, 0,    1};

  std::vector<float> Hpdata{1 - a_f,        a_z + ch_y, 0,       //
                            -(a_z + ch_y),  1 - a_f,    -f_a_x,  //
                            a_y_f - ch_z_f, -a_x_f,     1};

  cv::Mat HoffT(3, 3, CV_32F, Hoffdata.data());
  cv::Mat Hoff(3, 3, CV_32F, HoffTdata.data());
  cv::Mat H(3, 3, CV_32F, Hdata.data());
  cv::Mat Hp(3, 3, CV_32F, Hpdata.data());

  cv::Mat leftWarped = left.clone();
  cv::Mat rightWarped = right.clone();

  cv::warpPerspective(left, leftWarped, HoffT * H * Hoff, cv::Size(left.cols, left.rows));
  cv::warpPerspective(right, rightWarped, HoffT * Hp * Hoff, right.size());

  return {leftWarped, rightWarped};
}

void drawChosenMatches(const cv::Mat& left,
                       const cv::Mat& right,
                       const s3d::DisparityAnalyzerSTAN::Results& results) {
  cv::Mat leftRight(left.rows, left.cols * 2, left.type());
  cv::hconcat(left, right, leftRight);

  // convert to color
  cv::cvtColor(leftRight, leftRight, cv::COLOR_GRAY2BGR);

  auto ptsLeft = results.featurePointsLeft;
  auto ptsRight = results.featurePoints;

  // for random color
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(0, 255);

  for (int i = 0; i < results.featurePoints.size(); ++i) {
    // random color
    cv::Scalar color(dist(mt), dist(mt), dist(mt));

    auto& ptLeft = ptsLeft[i];
    Eigen::Vector2f ptRight = ptsRight[i] + Eigen::Vector2f(left.cols, 0.0f);
    cv::line(leftRight, cv::Point(ptLeft.x(), ptLeft.y()), cv::Point(ptRight.x(), ptRight.y()), color, 2);
    cv::circle(leftRight, cv::Point(ptLeft.x(), ptLeft.y()), 10, color);
    cv::circle(leftRight, cv::Point(ptRight.x(), ptRight.y()), 10, color);
  }

//  s3d::resizeMat(&leftRight, 1.0f / 2.0f);

  cv::imshow("concatened", leftRight);
}

int main(int argc, char* argv[]) {
  auto input_args = gsl::span<char*>(argv, argc);
  if (input_args.size() != 3) {
    throw BadNumberOfArgumentsException{};
  }

  // load images
  auto path_left = std::string(input_args[1]);
  auto path_right = std::string(input_args[2]);

  cv::Mat leftOrig = cv::imread(path_left, CV_LOAD_IMAGE_GRAYSCALE);
  cv::Mat rightOrig = cv::imread(path_right, CV_LOAD_IMAGE_GRAYSCALE);
  if (leftOrig.data == nullptr || rightOrig.data == nullptr) {
    throw FileNotFoundException{};
  }

  auto t1 = std::chrono::high_resolution_clock::now();

  // analyze disparities
  auto analyzer = std::make_unique<s3d::DisparityAnalyzerSTAN>(1.0f);
  analyzer->analyze(leftOrig, rightOrig);
  std::cout << analyzer->results;

  auto t2 = std::chrono::high_resolution_clock::now();
  std::cout << "Computation Time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms"
            << std::endl
            << std::endl;

  // disparity histogram
  auto&& disparitiesPercent = analyzer->results.disparitiesPercent;
//  auto hist = s3d::Histogram<float>::Compute(disparitiesPercent, 10);

  double minDispP, maxDispP;
  std::tie(minDispP, maxDispP) = s3d::disparity_range(disparitiesPercent);

  std::cout << "Disparity 1st percentile (%): " << minDispP << std::endl;
  std::cout << "Disparity 99th percentile (%): " << maxDispP << std::endl;

  // estimate pixel shift if current budget is considered adequate
  double wantedMinDisp = -1.0f;
  double wantedMaxDisp = 2.0f;
  double wantedCenter = (wantedMaxDisp + wantedMinDisp) / 2.0f;
  double currentCenter = (maxDispP + minDispP) / 2.0f;
  double shift = wantedCenter - currentCenter;

  std::cout << "Recommended shift (%) for " << wantedMinDisp << "% front, " << wantedMaxDisp
            << "% rear: " << shift << std::endl;
  std::cout << "Budget would become: " << minDispP + shift << ", " << maxDispP + shift << std::endl;

  //  vertical.addToAverage(model.ch_y); // %
  //  roll.addToAverage(model.a_z); // tan(model.a_z * 3.141592653589793 / 180.0) : degrees
  //  zoom.addToAverage(model.a_f); // (model.a_f + 1.0) * 100.0 : %
  //  tiltOffset.addToAverage(model.f_a_x); // pixels
  //  panKeystone.addToAverage(model.a_x_f);
  //  tiltKeystone.addToAverage(model.a_y_f);

  drawChosenMatches(leftOrig, rightOrig, analyzer->results);

  // draw epipolar lines
  std::vector<cv::Point2f> points1, points2;
  resultsToKeypoints(analyzer->results, &points1, &points2);
  auto F = fundamentalMatrixFromResults(analyzer->results);

  cv::Mat leftOrigEpipolar = leftOrig.clone();
  cv::Mat rightOrigEpipolar = leftOrig.clone();
  drawEpipolarLines2(leftOrigEpipolar, leftOrigEpipolar, rightOrigEpipolar, points1, points2, F, 1);
  cv::imshow("epipolar", rightOrigEpipolar);

  cv::Mat leftRect, rightRect;
  std::tie(leftRect, rightRect) = rectifyImages(leftOrig, rightOrig, analyzer->results.getStanAlignment());

  // resize for display
  s3d::resizeMat(&leftRect, 1.0f / 2.0f);
  s3d::resizeMat(&rightRect, 1.0f / 2.0f);

  cv::namedWindow("lefty");
  cv::namedWindow("righty");
  cv::imshow("lefty", leftRect);
  cv::imshow("righty", rightRect);
  cv::waitKey(0);

  return 0;
}
