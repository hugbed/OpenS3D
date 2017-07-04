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
}
