#ifndef WIDGETS_HITWIDGET_H
#define WIDGETS_HITWIDGET_H

#include <QDoubleSpinBox>

class DoubleSpinBoxNoCursor : public QDoubleSpinBox {
  Q_OBJECT
 public:
  explicit DoubleSpinBoxNoCursor(QWidget* parent = nullptr);
};

#endif  // WIDGETS_HITWIDGET_H
