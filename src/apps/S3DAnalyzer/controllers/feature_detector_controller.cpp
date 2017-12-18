#include <feature_detector_controller.h>

FeatureDetectorControllerBase::FeatureDetectorControllerBase(
  gsl::not_null<QAction*> action,
  gsl::not_null<s3d::DisparityAnalyzerSTAN*> disparityAnalyzer)
: m_action(action),
  m_disparityAnalyzer(disparityAnalyzer) {
  connect(m_action, &QAction::triggered, [this] {
    m_disparityAnalyzer->setMatchFinder(createMatchFinder());
  });
}