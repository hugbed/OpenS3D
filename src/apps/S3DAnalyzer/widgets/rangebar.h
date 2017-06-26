#ifndef WIDGETS_RANGEBAR_H
#define WIDGETS_RANGEBAR_H

#include <QWidget>
#include <QLabel>

#include <memory>

class RangeBar : public QWidget {
  Q_OBJECT
 public:
  explicit RangeBar(QWidget* parent = nullptr);

 protected:
  void paintEvent(QPaintEvent* event) override;

 public slots:
  void setLowValue(float lowValue);
  void setHighValue(float highValue);
  float clampValueInDisplayRange(float value);

  // min and max value displayed
  void setDisplayRange(float rangeMin, float rangeMax);

  // min and max values for gradiant (min = red, good = green, max = purple)
  void setExpectedRange(float rangeMin, float rangeMax);

 private:
  void drawBorder(QPainter* painter);
  void drawRange(QPainter* painter);
  void drawMiddleLine(QPainter* painter);

  QRectF expectedRangeRectangle();
  QRectF displayRangeRectangle();

  float m_borderMargin;
  float m_insideMargin;

  // todo : maybe put this in a struct
  float m_lowValue;
  float m_highValue;
  float m_displayRangeMin;
  float m_displayRangeMax;
  float m_expectedRangeMin;
  float m_expectedRangeMax;
};

#endif  // WIDGETS_RANGEBAR_H
