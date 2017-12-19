#ifndef WIDGETS_FILTERSETTINGSDIALOG_H
#define WIDGETS_FILTERSETTINGSDIALOG_H

#include <QDialog>
#include <QObject>

#include "ui_filtersettingsdialog.h"

#include <s3d/filter/stan_variance.h>

#include <memory>

class FilterSettingsDialog : public QDialog {
  Q_OBJECT

public:
  explicit FilterSettingsDialog(QWidget* parent = nullptr);

signals:
  void processVarianceChanged(const s3d::StanVariance& processVariance);

private:
  void connectSpinBox(QDoubleSpinBox* spinBox, double* value);
  void setInitialVarianceValuesFromSpinBoxes();

  s3d::StanVariance processVariance;
  std::unique_ptr<Ui::FilterSettingsDialog> ui;
};

#endif //WIDGETS_FILTERSETTINGSDIALOG_H
