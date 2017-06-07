#ifndef S3D_ANALYZER_WIDGETS_CONVERGENCEINDICATORWIDGET_H
#define S3D_ANALYZER_WIDGETS_CONVERGENCEINDICATORWIDGET_H

#include <QLabel>

#include <memory>

class WidthHintLabel : public QLabel {
  Q_OBJECT
 public:
  enum class State {
    ShouldNarrowVeryMuch,
    ShouldWidenVeryMuch,
    ShouldNarrowMuch,
    ShouldWidenMuch,
    ShouldNarrow,
    ShouldWiden,
    Neutral,
    NB_STATES
  };

  explicit WidthHintLabel(QWidget* parent = nullptr);

  void setState(State state);
  void updateHint(float ratio);

 private:
  void initPixmaps();
  void initPixmap(State state, const QString& filepath);

  bool inRange(float min, float max, float value);

  std::unique_ptr<QPixmap> m_pixmaps[static_cast<int>(State::NB_STATES)];
};

#endif  // S3D_ANALYZER_WIDGETS_CONVERGENCEINDICATORWIDGET_H
