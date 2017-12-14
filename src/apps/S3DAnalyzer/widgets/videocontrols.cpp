#include "videocontrols.h"

#include <QBoxLayout>
#include <QSlider>

#include "sliderdirectjump.h"
#include "videobuttons.h"

VideoControls::VideoControls(QWidget* parent) : QWidget(parent) {
  init(Qt::Horizontal);
}

VideoControls::VideoControls(Qt::Orientation orientation, QWidget* parent) : QWidget(parent) {
  init(orientation);
}

VideoControls::~VideoControls() = default;

void VideoControls::init(Qt::Orientation orientation) {
  m_videoButtons = std::make_unique<VideoButtons>(this);
  m_progressSlider = std::make_unique<SliderDirectJump>(Qt::Horizontal, this);

  connect(m_videoButtons.get(), &VideoButtons::firstClicked, this, &VideoControls::firstClicked);
  connect(m_videoButtons.get(), &VideoButtons::playClicked, this, &VideoControls::playClicked);
  connect(m_videoButtons.get(), &VideoButtons::pauseClicked, this, &VideoControls::pauseClicked);
  connect(m_videoButtons.get(), &VideoButtons::nextClicked, this, &VideoControls::nextClicked);
  connect(m_progressSlider.get(), &SliderDirectJump::valueClicked, [this](int value) {
    emit seekingRequested(std::chrono::microseconds{value});
  });

  auto layout = orientation == Qt::Horizontal ? std::unique_ptr<QBoxLayout>(new QHBoxLayout(this))
                                              : std::unique_ptr<QBoxLayout>(new QVBoxLayout(this));

  if (orientation == Qt::Vertical) {
    layout->addWidget(m_progressSlider.get());
    layout->addWidget(m_videoButtons.get());
    layout->setAlignment(m_videoButtons.get(), Qt::AlignCenter);
  } else {
    // horizontal
    layout->addWidget(m_videoButtons.get());
    layout->addWidget(m_progressSlider.get());
  }

  layout->addSpacerItem(new QSpacerItem(10, height()));
  layout->setMargin(0);

  // give up layout ownership
  m_layout = layout.get();
  setLayout(layout.release());

  using std::chrono_literals::operator""min;
  setDuration(11min);
}

void VideoControls::setDuration(std::chrono::microseconds duration) {
  m_progressSlider->setRange(0, duration.count());
}

void VideoControls::updateSlider(std::chrono::microseconds timestamp) {
  if (!m_progressSlider->isPressed()) {
    m_progressSlider->setValue(timestamp.count());
  }
}

void VideoControls::pause() {
  m_videoButtons->pause();
}
