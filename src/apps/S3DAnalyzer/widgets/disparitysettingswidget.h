#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

#include "utilities/usersettings.h"

#include <gsl/gsl>

#include <memory>

class QDoubleSpinBox;

namespace Ui {
class DisparitySettingsWidget;
}

class DisparitySettingsWidget : public QWidget {
  Q_OBJECT

 public:
  explicit DisparitySettingsWidget(QWidget* parent = 0);
  ~DisparitySettingsWidget() override;

  void setUserSettings(gsl::not_null<UserSettings*> settings);
  UserSettings getUserSettings();
  void conditionalAutoUpdate();

 signals:
  void settingsUpdated(UserSettings updatedSettings);
  void accepted();
  void rejected();

 private:
  void ensureMinSmallerThanMax(gsl::not_null<QDoubleSpinBox*> min,
                               gsl::not_null<QDoubleSpinBox*> max);

  void ensureMaxLargerThanMin(gsl::not_null<QDoubleSpinBox*> min,
                              gsl::not_null<QDoubleSpinBox*> max);

  std::unique_ptr<Ui::DisparitySettingsWidget> ui;
};

#endif  // SETTINGSWIDGET_H
