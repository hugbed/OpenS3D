#ifndef WIDGETS_DEPTHWIDGET_H
#define WIDGETS_DEPTHWIDGET_H

#include <QWidget>

#include <QLabel>

#include "depthvolumeindicator.h"

#include <memory>

class DepthWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DepthWidget(QWidget *parent = nullptr);

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
    std::unique_ptr<DepthVolumeIndicator> m_depthIndicator;
    std::unique_ptr<QLabel> m_maxValueLabel;

    float m_minValue;
    float m_maxValue;
    float m_shift;
};

#endif // DEPTHWIDGET_H
