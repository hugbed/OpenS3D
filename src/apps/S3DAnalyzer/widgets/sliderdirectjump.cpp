#include "sliderdirectjump.h"

#include <QMouseEvent>
#include <QStyleOptionSlider>

#include <cassert>

SliderDirectJump::SliderDirectJump(QWidget* parent) : QSlider(parent) {
  init();
}

SliderDirectJump::SliderDirectJump(Qt::Orientation orientation, QWidget* parent)
    : QSlider(orientation, parent) {
  // only horizontal supported for now
  assert(orientation == Qt::Horizontal);
  init();
}

SliderDirectJump::~SliderDirectJump() = default;

void SliderDirectJump::init() {
  // must be precise enough to jump under mouse
  setTickInterval(1);
  setPageStep(1);
  setRange(0, 10000);

  QStyleOptionSlider opt;
  initStyleOption(&opt);
  QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
  m_handleWidth = sr.width();
}

void SliderDirectJump::mousePressEvent(QMouseEvent* event) {
  m_isPressed = true;

  // only horizontal slider supported
  if (event->button() == Qt::LeftButton) {
    int min = minimum();
    int max = maximum();

    int value = computeHorizontalValue(event->x());
    setValue(value);
//    emit valueClicked(value);
    event->accept();
  }
  QSlider::mousePressEvent(event);
}

void SliderDirectJump::mouseReleaseEvent(QMouseEvent *event){
  // only horizontal slider supported
  if (event->button() == Qt::LeftButton) {
    int min = minimum();
    int max = maximum();

    int value = computeHorizontalValue(event->x());
    setValue(value);
    emit valueClicked(value);
    event->accept();
  }
  QSlider::mouseReleaseEvent(event);
  m_isPressed = false;
}

int SliderDirectJump::computeHorizontalValue(int x) {
  float halfHandleWidth = (0.5f * m_handleWidth) + 0.5f;  // Correct rounding

  float adaptedPosX = x;
  if (adaptedPosX < halfHandleWidth) {
    adaptedPosX = halfHandleWidth;
  } else if (adaptedPosX > width() - halfHandleWidth) {
    adaptedPosX = width() - halfHandleWidth;
  }

  // get new dimensions accounting for slider handle width
  float newWidth = (width() - halfHandleWidth) - halfHandleWidth;
  float normalizedPosition = (adaptedPosX - halfHandleWidth) / newWidth;
  int newVal = static_cast<int>(minimum() + ((maximum() - minimum()) * normalizedPosition));

  if (invertedAppearance()) {
    newVal = maximum() - newVal;
  }

  return newVal;
}

bool SliderDirectJump::isPressed() {
  return m_isPressed;
}
