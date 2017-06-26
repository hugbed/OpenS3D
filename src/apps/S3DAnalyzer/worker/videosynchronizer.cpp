#include "videosynchronizer.h"

#include <QTimer>
#include <QDebug>

#include <s3d/video/capture/file_video_capture_device.h>

VideoSynchronizer::VideoSynchronizer() {
  m_videoCaptureDevice = std::unique_ptr<VideoCaptureDevice>(
      std::make_unique<FileVideoCaptureDevice>("/home/jon/Videos/current-left.yuv")
  );

  m_timer = std::make_unique<QTimer>(this);
  m_timer->setTimerType(Qt::PreciseTimer);
  connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(checkForIncomingImage()));
  m_timer->start(33);

  VideoCaptureFormat format{};
  m_videoCaptureDevice->AllocateAndStart(format, this);
}

gsl::owner<VideoSynchronizer*> VideoSynchronizer::clone() const
{
  return new VideoSynchronizer;
}

VideoSynchronizer::~VideoSynchronizer() = default;

void VideoSynchronizer::OnIncomingCapturedData(const Images& data,
                                               const VideoCaptureFormat& frameFormat) {
  std::unique_lock<std::mutex>(m_mutex);
  // if previous frame not consumed, skip frame
  if (!m_imagesDirty) {
    m_imagesDirty = true;
    m_images = data;
  }
}

void VideoSynchronizer::checkForIncomingImage() {
  {
    std::unique_lock<std::mutex>(m_mutex);
    if (m_imagesDirty) {
      qDebug() << "new image yeah";
      // should use Qt::QueuedConnection for minimal time spent in timer callback
      emit incomingImagePair({}, {});
      m_imagesDirty = false;
    }
  }

}
