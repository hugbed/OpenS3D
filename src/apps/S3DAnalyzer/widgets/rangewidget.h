#ifndef WIDGETS_DEPTHWIDGET_H
#define WIDGETS_DEPTHWIDGET_H

#include <QWidget>

#include <QLabel>

#include "rangebar.h"

#include <memory>

class RangeWidget : public QWidget {
  Q_OBJECT
 public:
  explicit RangeWidget(QWidget* parent = nullptr);

 signals:

 public slots:
  void setLowValue(float lowValue);
  void setHighValue(float highValue);
  void updateLowValue();
  void updateHighValue();
  void setDisplayRange(float rangeMin, float rangeMax);
  void setExpectedRange(float rangeMin, float rangeMax);
  void setShift(float shift);

 private:
  QString formatNumberToString(float value);

  std::unique_ptr<QLabel> m_minValueLabel;
  std::unique_ptr<RangeBar> m_rangeBar;
  std::unique_ptr<QLabel> m_maxValueLabel;

  float m_minValue{0.0f};
  float m_maxValue{0.0f};
  float m_shift{0.0f};
};

#endif  // WIDGETS_DEPTHWIDGET_H
