#ifndef S3D_ANALYZER_WIDGETS_LINEEDITNOCURSOR_H
#define S3D_ANALYZER_WIDGETS_LINEEDITNOCURSOR_H

#include <QLineEdit>

class LineEditNoCursor : public QLineEdit {
  Q_OBJECT
 public:
  explicit LineEditNoCursor(QWidget* parent = nullptr);

  void keyPressEvent(QKeyEvent* e) override;
};

#endif  // S3D_ANALYZER_WIDGETS_LINEEDITNOCURSOR_H
