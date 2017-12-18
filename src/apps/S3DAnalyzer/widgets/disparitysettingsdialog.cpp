#include "disparitysettingsdialog.h"

#include <QVBoxLayout>

DisparitySettingsDialog::DisparitySettingsDialog(QWidget* parent)
    : QDialog(parent), m_settingsWidget{std::make_unique<DisparitySettingsWidget>()} {
  setWindowTitle("Settings");

  connect(m_settingsWidget.get(), &DisparitySettingsWidget::settingsUpdated, [this](UserSettings settings) {
    emit settingsUpdated(settings);
  });
  connect(m_settingsWidget.get(), &DisparitySettingsWidget::accepted, [this] { hide(); });
  connect(m_settingsWidget.get(), &DisparitySettingsWidget::rejected, [this] { hide(); });

  auto layout = std::make_unique<QVBoxLayout>();
  layout->addWidget(m_settingsWidget.get());
  setLayout(layout.release());
}

void DisparitySettingsDialog::setUserSettings(gsl::not_null<UserSettings*> userSettings) {
  m_settingsWidget->setUserSettings(userSettings);
}
