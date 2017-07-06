#include "videosynchronizer.h"

#include <QTimer>
#include <QDebug>

#include <s3d/video/capture/file_video_capture_device_ffmpeg.h>
#include <s3d/video/capture/file_video_capture_device_3d.h>

VideoSynchronizer::VideoSynchronizer() {
  m_videoCaptureDevice =
      std::unique_ptr<VideoCaptureDevice>(std::make_unique<FileVideoCaptureDevice3D>(
          "/home/jon/Videos/bbb_sunflower_1080p_30fps_stereo_left.mp4;"
          "/home/jon/Videos/bbb_sunflower_1080p_30fps_stereo_right.mp4"));

  m_images.resize(2);

  m_timer = std::make_unique<QTimer>(this);
  m_timer->setTimerType(Qt::PreciseTimer);
  connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(checkForIncomingImage()));
  m_timer->start(10);

  VideoCaptureFormat format{{}, -1, VideoPixelFormat::BGRA};
  m_videoCaptureDevice->AllocateAndStart(format, this);
}

gsl::owner<VideoSynchronizer*> VideoSynchronizer::clone() const {
  return new VideoSynchronizer;
}

VideoSynchronizer::~VideoSynchronizer() = default;

void VideoSynchronizer::resume() {
  m_videoCaptureDevice->Resume();
}

void VideoSynchronizer::pause() {
  m_videoCaptureDevice->MaybeSuspend();
}

void VideoSynchronizer::next() {
  m_videoCaptureDevice->RequestRefreshFrame();
}

void VideoSynchronizer::stop() {
  m_videoCaptureDevice->StopAndDeAllocate();
}

void VideoSynchronizer::OnIncomingCapturedData(const Images& data,
                                               const VideoCaptureFormat& frameFormat) {
  std::unique_lock<std::mutex>(m_mutex);
  // if previous frame not consumed, skip frame
  // copy data to a QImage
  m_images[0] = QImage(data[0].data(), 1920, 1080, QImage::Format_ARGB32).copy();
  m_images[1] = QImage(data[1].data(), 1920, 1080, QImage::Format_ARGB32).copy();
  m_imagesDirty = true;
}

void VideoSynchronizer::checkForIncomingImage() {
  std::unique_lock<std::mutex> lock(m_mutex);
  if (m_imagesDirty) {
    // should use Qt::QueuedConnection for minimal time spent in timer callback
    emit incomingImagePair(m_images[0], m_images[1]);
    m_imagesDirty = false;
  }
}
