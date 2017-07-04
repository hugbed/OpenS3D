#ifndef WIDGETS_VIDEOBUTTONS_H
#define WIDGETS_VIDEOBUTTONS_H

#include <QWidget>

#include <memory>

class QAbstractButton;
class QHBoxLayout;

class VideoButtons : public QWidget {
 public:
  VideoButtons(QWidget* parent = nullptr);
  ~VideoButtons();

  void togglePlayingState();

 private:
  std::unique_ptr<QAbstractButton> createButton(const QString& iconFileName);

  bool m_isPlaying{false};
  QHBoxLayout* m_layout;
  std::unique_ptr<QAbstractButton> m_firstButton;
  std::unique_ptr<QAbstractButton> m_previousButton;
  std::unique_ptr<QAbstractButton> m_playButton;
  std::unique_ptr<QAbstractButton> m_nextButton;
  std::unique_ptr<QAbstractButton> m_lastButton;
};

#endif  // WIDGETS_VIDEOBUTTONS_H
