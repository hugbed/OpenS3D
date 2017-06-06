#include "parameterwidget.h"

ParameterWidget::ParameterWidget(const QString &label, const QString& unitSymbol, QWidget *parent)
    : QWidget(parent)
    , m_unitSymbol(unitSymbol)
{
    setLabel(label);
    setValue(0.0f);

    m_label.setAlignment(Qt::AlignHCenter);
    m_value.setAlignment(Qt::AlignHCenter);

    setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));

    m_layout.addWidget(&m_value);
    m_layout.addWidget(&m_label);
    setLayout(&m_layout);
}

void ParameterWidget::setLabel(const QString &label)
{
    m_value.setText(label);
}

void ParameterWidget::setValue(float value)
{
    m_label.setText(QString::number(value, 'f', 2) + " " + m_unitSymbol);
}
