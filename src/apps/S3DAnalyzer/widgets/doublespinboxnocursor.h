#ifndef S3D_ANALYZER_WIDGETS_HITWIDGET_H
#define S3D_ANALYZER_WIDGETS_HITWIDGET_H

#include <QDoubleSpinBox>

class DoubleSpinBoxNoCursor : public QDoubleSpinBox {
  Q_OBJECT
 public:
  explicit DoubleSpinBoxNoCursor(QWidget* parent = nullptr);
};

#endif  // S3D_ANALYZER_WIDGETS_HITWIDGET_H
