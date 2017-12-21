#include "filtersettingsdialog.h"

FilterSettingsDialog::FilterSettingsDialog(QWidget* parent)
        : QDialog(parent),
          ui(std::make_unique<Ui::FilterSettingsDialog>()) {
  ui->setupUi(this);

  connect(ui->buttonBox, &QDialogButtonBox::accepted, [this] {
    hide();
  });

  setInitialVarianceValuesFromSpinBoxes();

  connectSpinBox(ui->rollSpinBox, &processVariance.roll);
  connectSpinBox(ui->verticalSpinBox, &processVariance.vertical);
  connectSpinBox(ui->panKeystoneSpinBox, &processVariance.panKeystone);
  connectSpinBox(ui->tiltKeystoneSpinBox, &processVariance.tiltKeystone);
  connectSpinBox(ui->tiltOffsetSpinBox, &processVariance.tiltOffset);
  connectSpinBox(ui->zoomSpinBox, &processVariance.zoom);

  connect(ui->startNoiseMesureButton, &QPushButton::pressed, [this] {
    measuringNoise = !measuringNoise;
    if (measuringNoise) {
      ui->startNoiseMesureButton->setText("Stop");
      emit startNoiseMeasure();
    } else {
      ui->startNoiseMesureButton->setText("Start");
      emit stopNoiseMeasure();
    }
  });
}

void FilterSettingsDialog::connectSpinBox(QDoubleSpinBox *spinBox, double *value) {
  connect(spinBox,
    static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
    [this, value](double newValue) {
      *value = newValue;
      emit processVarianceChanged(processVariance);
    });
}

void FilterSettingsDialog::setInitialVarianceValuesFromSpinBoxes() {
  processVariance.roll = ui->rollSpinBox->value();
  processVariance.vertical = ui->verticalSpinBox->value();
  processVariance.panKeystone = ui->panKeystoneSpinBox->value();
  processVariance.tiltKeystone = ui->tiltKeystoneSpinBox->value();
  processVariance.tiltOffset = ui->tiltOffsetSpinBox->value();
  processVariance.zoom = ui->zoomSpinBox->value();
}


