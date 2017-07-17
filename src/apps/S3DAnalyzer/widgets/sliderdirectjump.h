#ifndef WIDGETS_SLIDERDIRECTJUMP_H
#define WIDGETS_SLIDERDIRECTJUMP_H

#include <QSlider>

class QMouseEvent;

class SliderDirectJump : public QSlider {
 public:
  explicit SliderDirectJump(QWidget* parent = nullptr);
  explicit SliderDirectJump(Qt::Orientation orientation, QWidget* parent = nullptr);
  ~SliderDirectJump() override;

  void setPercentage(float percent);

 private:
  void init();

 protected:
  void mousePressEvent(QMouseEvent* event) override;

  int computeHorizontalValue(int x);

 private:
  int m_handleWidth{0};
};

#endif  // WIDGETS_SLIDERDIRECTJUMP_H
