#ifndef WIDGETS_SLIDERDIRECTJUMP_H
#define WIDGETS_SLIDERDIRECTJUMP_H

#include <QSlider>

class QMouseEvent;

class SliderDirectJump : public QSlider {
  Q_OBJECT

 public:
  explicit SliderDirectJump(QWidget* parent = nullptr);
  explicit SliderDirectJump(Qt::Orientation orientation, QWidget* parent = nullptr);
  ~SliderDirectJump() override;

  bool isPressed();

 signals:
  void valueClicked(int value);

 private:
  void init();

 protected:
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

  int computeHorizontalValue(int x);

 private:
  bool m_isPressed;
  int m_handleWidth{0};
};

#endif  // WIDGETS_SLIDERDIRECTJUMP_H
