#ifndef CONTROLLERS_FEATURE_DETECTOR_CONTROLLER_H
#define CONTROLLERS_FEATURE_DETECTOR_CONTROLLER_H

#include <QObject>

#include <QAction>

#include <s3d/cv/features/match_finder_cv.h>
#include <s3d/cv/disparity/disparity_analyzer_stan.h>

#include <gsl/gsl>

class FeatureDetectorControllerBase : QObject {
  Q_OBJECT

public:
  FeatureDetectorControllerBase(gsl::not_null<QAction*> action,
                                gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer);

  virtual std::unique_ptr<s3d::MatchFinderCV> createMatchFinder() = 0;

private:
  QAction* m_action;
  s3d::DisparityAnalyzerSTAN* m_disparityAnalyzer;
};

template <class T>
class FeatureDetectorController : public FeatureDetectorControllerBase {
  FeatureDetectorController(gsl::not_null<QAction*> action,
                            gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer)
    : FeatureDetectorControllerBase(action, disparityAnalyzer) {}

  std::unique_ptr<s3d::MatchFinderCV> createMatchFinder() override {
    return std::make_unique<T>();
  }
};


#endif //CONTROLLERS_FEATURE_DETECTOR_CONTROLLER_H
