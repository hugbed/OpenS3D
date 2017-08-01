#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

#include "utilities/usersettings.h"

#include <gsl/gsl>

#include <memory>

class QDoubleSpinBox;

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget {
  Q_OBJECT

 public:
  explicit SettingsWidget(QWidget* parent = 0);
  ~SettingsWidget() override;

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

  std::unique_ptr<Ui::SettingsWidget> ui;
};

#endif  // SETTINGSWIDGET_H
