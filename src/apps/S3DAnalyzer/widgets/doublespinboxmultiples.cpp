#include "doublespinboxmultiples.h"

DoubleSpinBoxMultiples::DoubleSpinBoxMultiples(QWidget* parent) : QDoubleSpinBox(parent) {}

void DoubleSpinBoxMultiples::stepBy(int steps)
{
  if (steps > 0) {
    setValue(value()*m_multiple);
  } else if (steps < 0) {
    setValue(value()/m_multiple);
  }
}