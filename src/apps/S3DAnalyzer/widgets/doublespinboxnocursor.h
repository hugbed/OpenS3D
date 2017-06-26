#ifndef WIDGETS_DOUBLESPINBOXNOCURSOR_H
#define WIDGETS_DOUBLESPINBOXNOCURSOR_H

#include <QDoubleSpinBox>

class DoubleSpinBoxNoCursor : public QDoubleSpinBox {
  Q_OBJECT
 public:
  explicit DoubleSpinBoxNoCursor(QWidget* parent = nullptr);
};

#endif  // WIDGETS_DOUBLESPINBOXNOCURSOR_H
