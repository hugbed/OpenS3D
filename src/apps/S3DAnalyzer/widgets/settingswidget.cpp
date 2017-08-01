#include "settingswidget.h"
#include "ui_settingswidget.h"

#include <QPushButton>

SettingsWidget::SettingsWidget(QWidget* parent)
    : QWidget(parent), ui{std::make_unique<Ui::SettingsWidget>()} {
  ui->setupUi(this);

  connect(ui->dialogButtonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
          [this] { emit settingsUpdated(getUserSettings()); });

  connect(ui->dialogButtonBox, &QDialogButtonBox::accepted, [this] {
    emit settingsUpdated(getUserSettings());
    emit accepted();
  });

  connect(ui->dialogButtonBox, &QDialogButtonBox::rejected, [this] { emit rejected(); });

  connect(ui->minDisplayRangeSpinBox,
          static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
          [this](double value) {
            ensureMinSmallerThanMax(ui->minDisplayRangeSpinBox, ui->maxDisplayRangeSpinBox);
            conditionalAutoUpdate();
          });

  connect(ui->maxDisplayRangeSpinBox,
          static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
          [this](double value) {
            ensureMaxLargerThanMin(ui->minDisplayRangeSpinBox, ui->maxDisplayRangeSpinBox);
            conditionalAutoUpdate();
          });

  connect(ui->minExpectedRangeSpinBox,
          static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
          [this](double value) {
            ensureMinSmallerThanMax(ui->minExpectedRangeSpinBox, ui->maxExpectedRangeSpinBox);
            conditionalAutoUpdate();
          });

  connect(ui->maxExpectedRangeSpinBox,
          static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
          [this](double value) {
            ensureMaxLargerThanMin(ui->minExpectedRangeSpinBox, ui->maxExpectedRangeSpinBox);
            conditionalAutoUpdate();
          });

  connect(ui->viewerDistanceLabelSpinBox,
          static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
          [this](double value) { conditionalAutoUpdate(); });

  connect(ui->interocularDistanceSpinBox,
          static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
          [this](double value) { conditionalAutoUpdate(); });

  connect(ui->screenWidthSpinBox,
          static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
          [this](double value) { conditionalAutoUpdate(); });

  connect(ui->displayZoomSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
          [this](int value) { conditionalAutoUpdate(); });
}

SettingsWidget::~SettingsWidget() = default;

void SettingsWidget::setUserSettings(gsl::not_null<UserSettings*> settings) {
  ui->minDisplayRangeSpinBox->setValue(settings->displayParameters.displayRangeMin);
  ui->maxDisplayRangeSpinBox->setValue(settings->displayParameters.displayRangeMax);
  ui->minExpectedRangeSpinBox->setValue(settings->displayParameters.expectedRangeMin);
  ui->maxExpectedRangeSpinBox->setValue(settings->displayParameters.expectedRangeMax);
  ui->viewerDistanceLabelSpinBox->setValue(settings->viewerContext.viewerDistance);
  ui->interocularDistanceSpinBox->setValue(settings->viewerContext.interocularDistance);
  ui->screenWidthSpinBox->setValue(settings->viewerContext.screenWidth);
  ui->displayZoomSpinBox->setValue(static_cast<int>(settings->viewerDisplayZoom * 100.0f));
}

UserSettings SettingsWidget::getUserSettings() {
  DisplayParameters displayParameters;
  displayParameters.displayRangeMin = static_cast<float>(ui->minDisplayRangeSpinBox->value());
  displayParameters.displayRangeMax = static_cast<float>(ui->maxDisplayRangeSpinBox->value());
  displayParameters.expectedRangeMin = static_cast<float>(ui->minExpectedRangeSpinBox->value());
  displayParameters.expectedRangeMax = static_cast<float>(ui->maxExpectedRangeSpinBox->value());

  s3d::ViewerContext viewerContext(static_cast<float>(ui->viewerDistanceLabelSpinBox->value()),
                                   static_cast<float>(ui->interocularDistanceSpinBox->value()),
                                   static_cast<float>(ui->screenWidthSpinBox->value()), 0);

  UserSettings settings;
  settings.displayParameters = displayParameters;
  settings.viewerContext = viewerContext;
  settings.viewerDisplayZoom = static_cast<float>(ui->displayZoomSpinBox->value()) / 100.0f;

  // move to disparity hints class or something (recommended settings)
  if (ui->autoMinExpectedRangeCheckBox->isChecked()) {
    settings.displayParameters.expectedRangeMax =
        viewerContext.interocularDistance / viewerContext.screenWidth * 100.0f;
    ui->maxExpectedRangeSpinBox->setValue(settings.displayParameters.expectedRangeMax);
  }

  return settings;
}

void SettingsWidget::conditionalAutoUpdate() {
  if (ui->automaticUpdateCheckBox->isChecked()) {
    emit settingsUpdated(getUserSettings());
  }
}

void SettingsWidget::ensureMinSmallerThanMax(gsl::not_null<QDoubleSpinBox*> min,
                                             gsl::not_null<QDoubleSpinBox*> max) {
  if (min->value() >= max->value()) {
    min->setValue(max->value() - min->singleStep());
  }
}

void SettingsWidget::ensureMaxLargerThanMin(gsl::not_null<QDoubleSpinBox*> min,
                                            gsl::not_null<QDoubleSpinBox*> max) {
  if (min->value() >= max->value()) {
    max->setValue(min->value() + max->singleStep());
  }
}
