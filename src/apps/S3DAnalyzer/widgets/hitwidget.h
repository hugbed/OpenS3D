#ifndef WIDGETS_HITWIDGET_H
#define WIDGETS_HITWIDGET_H

#include <QDoubleSpinBox>

class HITWidget : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit HITWidget(QWidget *parent = nullptr);
};

#endif // WIDGETS_HITWIDGET_H
