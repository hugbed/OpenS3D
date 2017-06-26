#ifndef WIDGETS_LINEEDITNOCURSOR_H
#define WIDGETS_LINEEDITNOCURSOR_H

#include <QLineEdit>

class LineEditNoCursor : public QLineEdit {
  Q_OBJECT
 public:
  explicit LineEditNoCursor(QWidget* parent = nullptr);

  void keyPressEvent(QKeyEvent* e) override;
};

#endif  // WIDGETS_LINEEDITNOCURSOR_H
