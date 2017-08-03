#include "settingsdialog.h"

#include <QVBoxLayout>

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent), m_settingsWidget{std::make_unique<SettingsWidget>()} {
  setWindowTitle("Settings");

  connect(m_settingsWidget.get(), &SettingsWidget::settingsUpdated,
          [this](UserSettings settings) { emit settingsUpdated(settings); });
  connect(m_settingsWidget.get(), &SettingsWidget::accepted, [this] { hide(); });
  connect(m_settingsWidget.get(), &SettingsWidget::rejected, [this] { hide(); });

  auto layout = std::make_unique<QVBoxLayout>();
  layout->addWidget(m_settingsWidget.get());
  setLayout(layout.release());
}

void SettingsDialog::setUserSettings(gsl::not_null<UserSettings*> userSettings) {
  m_settingsWidget->setUserSettings(userSettings);
}
