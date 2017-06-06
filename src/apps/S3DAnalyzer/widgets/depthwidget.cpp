#include "depthwidget.h"

#include <QHBoxLayout>

DepthWidget::DepthWidget(QWidget *parent) : QWidget(parent)
{
    m_minValueLabel = std::make_unique<QLabel>();
    m_minValueLabel->setFixedWidth(30);
    m_depthIndicator = std::make_unique<DepthVolumeIndicator>();
    m_maxValueLabel = std::make_unique<QLabel>();
    m_maxValueLabel->setFixedWidth(30);

    setDisplayRange(-5.0f, 5.0f);
    setExpectedRange(-1.1f, 3.1f);
    setLowValue(-3.8);
    setHighValue(-0.8);

    auto layout = std::make_unique<QHBoxLayout>();
    layout->addWidget(m_minValueLabel.get(), 0, Qt::AlignLeft);
    layout->addWidget(m_depthIndicator.get(), 0, Qt::AlignCenter);
    layout->addWidget(m_maxValueLabel.get(), 0, Qt::AlignRight);
    setLayout(layout.release());
}


void DepthWidget::setLowValue(float lowValue)
{
    m_minValue = lowValue;
    updateLowValue();
}

void DepthWidget::setHighValue(float highValue)
{
    m_maxValue = highValue;
    updateHighValue();
}

void DepthWidget::updateLowValue()
{
    float value = m_minValue + m_shift;
    m_minValueLabel->setText(formatNumberToString(value));
    m_depthIndicator->setLowValue(value);
}

void DepthWidget::updateHighValue()
{
    float value = m_maxValue + m_shift;
    m_maxValueLabel->setText(formatNumberToString(value));
    m_depthIndicator->setHighValue(value);
}

void DepthWidget::setDisplayRange(float rangeMin, float rangeMax)
{
    m_depthIndicator->setDisplayRange(rangeMin, rangeMax);
}

void DepthWidget::setExpectedRange(float rangeMin, float rangeMax)
{
    m_depthIndicator->setExpectedRange(rangeMin, rangeMax);
}

void DepthWidget::setShift(float shift)
{
    m_shift = shift;
    updateLowValue();
    updateHighValue();
}

QString DepthWidget::formatNumberToString(float value)
{
    return QString::number(value, 'f', 1);
}
