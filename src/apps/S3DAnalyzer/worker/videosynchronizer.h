#ifndef WORKER_VIDEOSYNCHRONIZER_H
#define WORKER_VIDEOSYNCHRONIZER_H

#include <s3d/video/capture/video_capture_device.h>

#include <QImage>
#include <QObject>

#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>

class QTimer;

namespace s3d {
class StereoDemuxer;
class StereoDemuxerFactory;
}  // namespace s3d

// synchronizes video acquisition in another thread
// with the main thread using a timer to check when
// a new frame is ready

class VideoSynchronizer : public QObject, public s3d::VideoCaptureDevice::Client {
  Q_OBJECT

 public:
  VideoSynchronizer();
  ~VideoSynchronizer() override;

  gsl::owner<VideoSynchronizer*> clone() const override;

  void setStereoVideoFormat(s3d::Stereo3DFormat format);
  void setLeftFilename(std::string filename);
  void setRightFilename(std::string filename);

  void loadStereoVideo(const std::string& leftFile,
                       const std::string& rightFile,
                       s3d::Stereo3DFormat stereoFormat);

  void loadStereoVideo();
  void loadLiveCamera();

  void resume();
  void pause();
  void next();
  void stop();
  void seekTo(std::chrono::microseconds timestamp);

  bool isVideoLoaded();
  bool isVideoReadyToLoad();

  std::chrono::microseconds videoDuration();

  void OnIncomingCapturedData(const Images& data,
                              const s3d::VideoCaptureFormat& frameFormat,
                              std::chrono::microseconds timestamp) override;

 signals:
  void incomingImagePair(const QImage& left,
                         const QImage& right,
                         std::chrono::microseconds timestamp);

 public slots:
  void checkForIncomingImage();

 private:
  std::unique_ptr<QTimer> createAndStartTimer();

  bool stereoFormatChanged();
  void updateStereoDemuxer(const s3d::VideoCaptureFormat& format);
  bool stereoDemuxerRequired();

  s3d::Stereo3DFormat m_stereoFormat{s3d::Stereo3DFormat::Separate};
  std::string m_leftFilename{"/home/jon/Videos/bbb_sunflower_1080p_30fps_stereo_left.mp4"};
  std::string m_rightFilename{"/home/jon/Videos/bbb_sunflower_1080p_30fps_stereo_right.mp4"};

  std::unique_ptr<s3d::VideoCaptureDevice> m_videoCaptureDevice;
  std::unique_ptr<s3d::StereoDemuxerFactory> m_stereoDemuxerFactory;
  std::unique_ptr<s3d::StereoDemuxer> m_stereoDemuxer;
  std::unique_ptr<QTimer> m_timer;
  std::chrono::microseconds m_videoDuration{};

  bool m_leftFileReady{false};
  bool m_rightFileReady{false};
  bool m_videoLoaded{false};

  // synchronization
  std::mutex m_mutex;
  bool m_imagesDirty{false};
  std::vector<QImage> m_images;
  std::chrono::microseconds m_timestamp;
};

#endif  // WORKER_VIDEOSYNCHRONIZER_H
