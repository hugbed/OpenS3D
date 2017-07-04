#ifndef WORKER_VIDEOSYNCHRONIZER_H
#define WORKER_VIDEOSYNCHRONIZER_H

#include <s3d/video/capture/video_capture_device.h>

#include <QObject>
#include <QImage>

#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>

class QTimer;

// synchronizes video acquisition in another thread
// with the main thread using a timer to check when
// a new frame is ready

class VideoSynchronizer : public QObject, public VideoCaptureDevice::Client {
  Q_OBJECT

 public:
  VideoSynchronizer();
  ~VideoSynchronizer();

  gsl::owner<VideoSynchronizer*> clone() const override;

  void stop();

  void OnIncomingCapturedData(const Images& data, const VideoCaptureFormat& frameFormat) override;

 signals:
  void incomingImagePair(const QImage& left, const QImage& right);

 public slots:
  void checkForIncomingImage();

 private:
  std::unique_ptr<VideoCaptureDevice> m_videoCaptureDevice;
  std::unique_ptr<QTimer> m_timer;

  // synchronization
  std::mutex m_mutex;
  bool m_imagesDirty{false};
  std::vector<gsl::span<const uint8_t>> m_images;
};

#endif  // WORKER_VIDEOSYNCHRONIZER_H
