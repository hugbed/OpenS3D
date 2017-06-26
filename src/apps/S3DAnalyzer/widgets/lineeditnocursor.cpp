#include "lineeditnocursor.h"

LineEditNoCursor::LineEditNoCursor(QWidget* parent) : QLineEdit(parent) {
  setReadOnly(true);
}

void LineEditNoCursor::keyPressEvent(QKeyEvent* e) {
  setReadOnly(false);
  QLineEdit::keyPressEvent(e);
  setReadOnly(true);
}
