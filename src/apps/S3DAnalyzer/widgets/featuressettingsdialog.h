#ifndef WIDGETS_FEATURESSETTINGSDIALOG_H
#define WIDGETS_FEATURESSETTINGSDIALOG_H

#include <QDialog>
#include <QObject>

#include <memory>

#include "ui_featuressettingsdialog.h"

namespace s3d {
class MatchFinderCV;
}

class MatchFinderFromIndexFactory {
public:
  static std::unique_ptr<s3d::MatchFinderCV> create(int index);
};

class FeaturesSettingsDialog : public QDialog {
  Q_OBJECT

public:
  explicit FeaturesSettingsDialog(QWidget* parent = nullptr);

signals:
  void imageScalingRatioChanged(float newScaleRatio);
  void maxNbFeaturesChanged(int newMaxNbFeatures);
  void featureDetectorChanged(int newDetectorIndex);

private:
  std::unique_ptr<Ui::FeaturesSettingsDialog> ui;
};

#endif //WIDGETS_FEATURESSETTINGSDIALOG_H
