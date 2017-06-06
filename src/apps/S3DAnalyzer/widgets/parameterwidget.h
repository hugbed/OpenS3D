#ifndef WIDGETS_PARAMETERWIDGET_H
#define WIDGETS_PARAMETERWIDGET_H

#include <QPushButton>

#include <QLabel>
#include <QVBoxLayout>

class ParameterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParameterWidget(const QString& label, const QString& unitSymbol, QWidget *parent = nullptr);

signals:

public slots:
    void setLabel(const QString& label);
    void setValue(float value);

private:
    QLabel m_label;
    QLabel m_value;
    QVBoxLayout m_layout;

    QString m_unitSymbol{""};
};

#endif // WIDGETS_PARAMETERWIDGET_H
