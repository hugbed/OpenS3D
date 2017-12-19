#ifndef WIDGETS_DOUBLESPINBOXMULTIPLES_H
#define WIDGETS_DOUBLESPINBOXMULTIPLES_H

#include <QDoubleSpinBox>

class DoubleSpinBoxMultiples : public QDoubleSpinBox {
public:
  explicit DoubleSpinBoxMultiples(QWidget* parent = nullptr);

public slots:
  void stepBy(int steps) override;

  double m_multiple{10.0};
};

#endif //WIDGETS_DOUBLESPINBOXMULTIPLES_H
