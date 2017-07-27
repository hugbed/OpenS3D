#ifndef WIDGETS_VIDEOCONTROLS_H
#define WIDGETS_VIDEOCONTROLS_H

#include <QWidget>

#include <chrono>
#include <memory>

class VideoButtons;
class QAbstractSlider;
class QBoxLayout;
class SliderDirectJump;

class VideoControls : public QWidget {
  Q_OBJECT
 public:
  explicit VideoControls(QWidget* parent = nullptr);
  explicit VideoControls(Qt::Orientation orientation, QWidget* parent = nullptr);
  ~VideoControls() override;

  void setDuration(std::chrono::microseconds duration);
  void updateSlider(std::chrono::microseconds timestamp);

 signals:
  void first();
  void play();
  void pause();
  void next();
  void seekingRequested(std::chrono::microseconds timestamp);

 private:
  void init(Qt::Orientation orientation);

  QBoxLayout* m_layout;
  std::unique_ptr<VideoButtons> m_videoButtons;
  std::unique_ptr<SliderDirectJump> m_progressSlider;
};

#endif  // WIDGETS_VIDEOCONTROLS_H
