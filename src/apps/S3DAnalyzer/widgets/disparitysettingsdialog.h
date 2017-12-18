#ifndef WIDGETS_SETTINGSDIALOG_H
#define WIDGETS_SETTINGSDIALOG_H

#include <QDialog>

#include "utilities/usersettings.h"
#include "widgets/disparitysettingswidget.h"

#include <memory>

class DisparitySettingsDialog : public QDialog {
  Q_OBJECT

 public:
  explicit DisparitySettingsDialog(QWidget* parent = nullptr);
  void setUserSettings(gsl::not_null<UserSettings*> userSettings);

 signals:
  void settingsUpdated(UserSettings userSettings);

 private:
  std::unique_ptr<DisparitySettingsWidget> m_settingsWidget;
};

#endif  // WIDGETS_SETTINGSDIALOG_H
