#ifndef PROJECT_SLIDERDIRECTJUMP_H
#define PROJECT_SLIDERDIRECTJUMP_H

#include <QSlider>

class QMouseEvent;

class SliderDirectJump : public QSlider {
 public:
  SliderDirectJump(QWidget* parent = nullptr);
  SliderDirectJump(Qt::Orientation orientation, QWidget* parent = nullptr);
  ~SliderDirectJump();

 private:
  void init();

 protected:
  void mousePressEvent(QMouseEvent* event) override;

  int computeHorizontalValue(int x);

 private:
  int m_handleWidth{0};
};

#endif  // PROJECT_SLIDERDIRECTJUMP_H
