#include "rangewidget.h"

#include <QHBoxLayout>

RangeWidget::RangeWidget(QWidget* parent) : QWidget(parent) {
  m_minValueLabel = std::make_unique<QLabel>();
  m_minValueLabel->setFixedWidth(30);
  m_rangeBar = std::make_unique<RangeBar>();
  m_maxValueLabel = std::make_unique<QLabel>();
  m_maxValueLabel->setFixedWidth(30);

  // default values, will be replaced
  setDisplayRange(-5.0f, 5.0f);
  setExpectedRange(-1.1f, 3.1f);
  setLowValue(-3.8);
  setHighValue(-0.8);

  auto layout = std::make_unique<QHBoxLayout>();
  layout->addWidget(m_minValueLabel.get(), 0, Qt::AlignLeft);
  layout->addWidget(m_rangeBar.get(), 0, Qt::AlignCenter);
  layout->addWidget(m_maxValueLabel.get(), 0, Qt::AlignRight);
  setLayout(layout.release());
}

void RangeWidget::setLowValue(float lowValue) {
  m_minValue = lowValue;
  updateLowValue();
}

void RangeWidget::setHighValue(float highValue) {
  m_maxValue = highValue;
  updateHighValue();
}

void RangeWidget::updateLowValue() {
  float value = m_minValue + m_shift;
  m_minValueLabel->setText(formatNumberToString(value));
  m_rangeBar->setLowValue(value);
}

void RangeWidget::updateHighValue() {
  float value = m_maxValue + m_shift;
  m_maxValueLabel->setText(formatNumberToString(value));
  m_rangeBar->setHighValue(value);
}

void RangeWidget::setDisplayRange(float rangeMin, float rangeMax) {
  m_rangeBar->setDisplayRange(rangeMin, rangeMax);
}

void RangeWidget::setExpectedRange(float rangeMin, float rangeMax) {
  m_rangeBar->setExpectedRange(rangeMin, rangeMax);
}

void RangeWidget::setShift(float shift) {
  m_shift = shift;
  updateLowValue();
  updateHighValue();
}

QString RangeWidget::formatNumberToString(float value) {
  return QString::number(value, 'f', 1);
}
