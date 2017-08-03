#include "videobuttons.h"

#include <QBoxLayout>
#include <QPushButton>

VideoButtons::VideoButtons(QWidget* parent) : QWidget(parent) {
  // todo: maybe not all of this is necessary
  QString style(
      "QPushButton"
      "{"
      "   color: #eff0f1;"
      "   background-color: #31363b;"
      "   border-width: 1px;"
      "   border-color: #76797C;"
      "   border-style: solid;"
      "   padding: 5px;"
      "   border-radius: 2px;"
      "   outline: none;"
      "}"
      ""
      "QPushButton:disabled"
      "{"
      "   background-color: #31363b;"
      "   border-width: 1px;"
      "   border-color: #454545;"
      "   border-style: solid;"
      "   padding-top: 5px;"
      "   padding-bottom: 5px;"
      "   padding-left: 10px;"
      "   padding-right: 10px;"
      "   border-radius: 2px;"
      "   color: #454545;"
      "}"
      ""
      "QPushButton:focus {"
      "   background-color: #31363b;"
      "   color: white;"
      "}"
      ""
      "QPushButton:pressed"
      "{"
      "   background-color: #31363b;"
      "   padding-top: -15px;"
      "   padding-bottom: -17px;"
      "}");

  setStyleSheet(style);

  auto layout = std::make_unique<QHBoxLayout>();

  m_firstButton = createButton(":icons/player_first.png");
  //  m_previousButton = createButton(":icons/player_previous.png");
  m_playButton = createButton(":icons/player_play.png");
  m_nextButton = createButton(":icons/player_next.png");
  //  m_lastButton = createButton(":icons/player_last.png");

  connect(m_firstButton.get(), &QAbstractButton::clicked, this, &VideoButtons::firstClicked);
  connect(m_playButton.get(), &QAbstractButton::clicked, this, &VideoButtons::togglePlayingState);
  connect(m_nextButton.get(), &QAbstractButton::clicked, this, &VideoButtons::nextClicked);

  layout->addWidget(m_firstButton.get());
  //  layout->addWidget(m_previousButton.get());
  layout->addWidget(m_playButton.get());
  layout->addWidget(m_nextButton.get());
  //  layout->addWidget(m_lastButton.get());

  // give up layout ownership
  m_layout = layout.get();
  m_layout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
  setLayout(layout.release());
}

VideoButtons::~VideoButtons() = default;

std::unique_ptr<QAbstractButton> VideoButtons::createButton(const QString& iconFileName) {
  std::unique_ptr<QAbstractButton> button = std::make_unique<QPushButton>();

  QPixmap pixmap(iconFileName);
  QIcon buttonIcon(pixmap);
  button->setIcon(buttonIcon);
  button->setIconSize(pixmap.size());
  button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  button->setCheckable(false);

  return button;
}

void VideoButtons::togglePlayingState() {
  if (m_isPlaying) {
    pause();
    emit pauseClicked();
  } else {
    play();
    emit playClicked();
  }
}

void VideoButtons::pause() {
  QPixmap pixmap(":icons/player_play.png");
  QIcon buttonIcon(pixmap);
  m_playButton->setIcon(buttonIcon);
  m_isPlaying = false;
}

void VideoButtons::play() {
  QPixmap pixmap(":icons/player_pause.png");
  QIcon buttonIcon(pixmap);
  m_playButton->setIcon(buttonIcon);
  m_isPlaying = true;
}
