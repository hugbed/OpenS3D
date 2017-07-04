#ifndef WIDGETS_VIDEOCONTROLS_H
#define WIDGETS_VIDEOCONTROLS_H

#include <QWidget>

#include <memory>

class VideoButtons;
class QAbstractSlider;
class QBoxLayout;

class VideoControls : public QWidget {
 public:
  VideoControls(QWidget* parent = nullptr);
  VideoControls(Qt::Orientation orientation, QWidget* parent = nullptr);
  ~VideoControls();

 private:
  void init(Qt::Orientation orientation);

  QBoxLayout* m_layout;
  std::unique_ptr<VideoButtons> m_videoButtons;
  std::unique_ptr<QAbstractSlider> m_progressSlider;
};

#endif  // WIDGETS_VIDEOCONTROLS_H
