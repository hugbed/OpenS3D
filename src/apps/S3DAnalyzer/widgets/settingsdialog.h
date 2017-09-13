#ifndef WIDGETS_SETTINGSDIALOG_H
#define WIDGETS_SETTINGSDIALOG_H

#include <QDialog>

#include "utilities/usersettings.h"
#include "widgets/settingswidget.h"

#include <memory>

class SettingsDialog : public QDialog {
  Q_OBJECT

 public:
  SettingsDialog(QWidget* parent = nullptr);
  void setUserSettings(gsl::not_null<UserSettings*> userSettings);

 signals:
  void settingsUpdated(UserSettings userSettings);

 private:
  std::unique_ptr<SettingsWidget> m_settingsWidget;
};

#endif  // WIDGETS_SETTINGSDIALOG_H
