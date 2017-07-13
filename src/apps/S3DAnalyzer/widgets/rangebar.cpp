#include "rangebar.h"

#include <QPainter>

#include "utilities/depthintensitygradient.h"

// todo : validate with values that don't make sense

RangeBar::RangeBar(QWidget* parent)
    : QWidget(parent),
      m_borderMargin(0.5f),
      m_lowValue{},
      m_highValue{},
      // default values, will be replaced
      m_displayRangeMin(-3.0f),
      m_displayRangeMax(3.0f),
      m_expectedRangeMin(-0.7f),
      m_expectedRangeMax(1.0f) {
  m_insideMargin = m_borderMargin + 1.0f;
  setFixedSize(300, 30);
  update();
}

void RangeBar::paintEvent(QPaintEvent* /*event*/) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  drawBorder(&painter);
  drawRange(&painter);
  drawMiddleLine(&painter);
}

void RangeBar::drawBorder(QPainter* painter) {
  QSizeF borderMargin(m_borderMargin, m_borderMargin);
  QSizeF borderSize(size() - borderMargin);

  QPainterPath path;
  QRectF borderRectangle(QPointF(borderMargin.width(), borderMargin.height()), borderSize);
  path.addRoundedRect(borderRectangle, 5.0f, 5.0f);

  QPen pen(QColor("#76797C"));
  painter->setPen(pen);
  painter->drawPath(path);
}

void RangeBar::drawRange(QPainter* painter) {
  QPainterPath path;

  QRectF rangeRect = displayRangeRectangle();
  path.addRoundedRect(rangeRect, 5.0f, 5.0f);

  // todo : put this gradient in another class maybe
  // to be able to set depth color from disparity
  // and to be able to access the same red/green/etc everywhere
  QRectF gradientRect = expectedRangeRectangle();
  DepthIntensityGradient gradient(gradientRect.topLeft(), gradientRect.bottomRight());
  painter->fillPath(path, gradient);
}

void RangeBar::drawMiddleLine(QPainter* painter) {
  float middleLineRatio = -m_displayRangeMin / (m_displayRangeMax - m_displayRangeMin);

  // quit if outside
  if (middleLineRatio < 0.0f || middleLineRatio > 1.0f) {
    return;
  }

  painter->save();
  {
    painter->translate(middleLineRatio * static_cast<float>(width()), m_insideMargin);

    float insideHeight = static_cast<float>(height()) - m_insideMargin * 2.0f;

    QPen pen(QColor("#76797C"), 2.0f);
    painter->setPen(pen);
    painter->drawRect({QPointF(-1.0f, 0.0f), QPointF(2.0f, insideHeight)});

    pen.setColor(Qt::gray);
    painter->setPen(pen);
    painter->drawLine(QPointF(0.0f, 0.5f), QPointF(0.0f, insideHeight - 0.5f));
  }
  painter->restore();
}

void RangeBar::setLowValue(float lowValue) {
  m_lowValue = lowValue;
  update();
}

void RangeBar::setHighValue(float highValue) {
  m_highValue = highValue;
  update();
}

void RangeBar::setDisplayRange(float rangeMin, float rangeMax) {
  m_displayRangeMin = rangeMin;
  m_displayRangeMax = rangeMax;
  update();
}

void RangeBar::setExpectedRange(float rangeMin, float rangeMax) {
  m_expectedRangeMin = rangeMin;
  m_expectedRangeMax = rangeMax;
  update();
}

float RangeBar::clampValueInDisplayRange(float value) {
  return std::max(m_displayRangeMin, std::min(value, m_displayRangeMax));
}

QRectF RangeBar::expectedRangeRectangle() {
  float totalRange = m_displayRangeMax - m_displayRangeMin;
  if (totalRange < 0.000001) {
    return {};
  }

  float expectedRange = m_expectedRangeMax - m_expectedRangeMin;
  auto totalWidth = static_cast<float>(width());
  float expectedWidth = totalWidth * expectedRange / totalRange;
  float lowPos = totalWidth * (m_expectedRangeMin - m_displayRangeMin) / totalRange;

  QPointF marginSize(m_insideMargin, m_insideMargin);
  return QRectF(QPointF(lowPos, 0) + marginSize,
                QPointF(lowPos + expectedWidth - m_insideMargin, 0));
}

// todo : unit test this
QRectF RangeBar::displayRangeRectangle() {
  float totalRange = m_displayRangeMax - m_displayRangeMin;
  if (totalRange < 0.000001) {
    return {};
  }

  // restrict values to display range
  float lowValue = clampValueInDisplayRange(m_lowValue);
  float highValue = clampValueInDisplayRange(m_highValue);

  float valueRange = highValue - lowValue;
  auto totalWidth = static_cast<float>(width());
  float valueWidth = valueRange * totalWidth / totalRange;
  float lowPos = totalWidth * (lowValue - m_displayRangeMin) / totalRange;

  auto widgetHeight = static_cast<float>(height());
  QPointF marginSize(m_insideMargin, m_insideMargin);
  return QRectF(QPointF(lowPos, 0) + marginSize,
                QPointF(lowPos + valueWidth, widgetHeight) - marginSize);
}
