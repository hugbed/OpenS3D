#include "videosynchronizer.h"

#include <QTimer>
#include <QDebug>

#include <s3d/video/capture/file_video_capture_device_3d.h>
#include <s3d/video/capture/file_video_capture_device_ffmpeg.h>
#include <s3d/video/file_parser/ffmpeg/video_file_parser_ffmpeg.h>
#include <s3d/video/stereo_demuxer/stereo_demuxer.h>
#include <s3d/cv/video/stereo_demuxer/stereo_demuxer_cv_side_by_side.h>
#include <s3d/cv/video/stereo_demuxer/stereo_demuxer_factory_cv.h>

VideoSynchronizer::VideoSynchronizer()
    : m_videoCaptureDevice{},
      m_stereoDemuxerFactory{std::make_unique<s3d::StereoDemuxerFactoryCV>()} {}

gsl::owner<VideoSynchronizer*> VideoSynchronizer::clone() const {
  return new VideoSynchronizer;
}

VideoSynchronizer::~VideoSynchronizer() = default;

void VideoSynchronizer::resume() {
  if (m_videoCaptureDevice != nullptr) {
    m_videoCaptureDevice->Resume();
  }
}

void VideoSynchronizer::pause() {
  if (m_videoCaptureDevice != nullptr) {
    m_videoCaptureDevice->MaybeSuspend();
  }
}

void VideoSynchronizer::next() {
  if (m_videoCaptureDevice != nullptr) {
    m_videoCaptureDevice->RequestRefreshFrame();
  }
}

void VideoSynchronizer::stop() {
  if (m_videoCaptureDevice != nullptr) {
    m_videoCaptureDevice->StopAndDeAllocate();
    m_videoLoaded = false;
  }
}

void VideoSynchronizer::seekTo(std::chrono::microseconds timestamp) {
  if (m_videoCaptureDevice != nullptr) {
    m_videoCaptureDevice->MaybeSeekTo(timestamp);
  }
}

std::chrono::microseconds VideoSynchronizer::videoDuration() {
  return m_videoDuration;
}

void VideoSynchronizer::OnIncomingCapturedData(const Images& data,
                                               const s3d::VideoCaptureFormat& frameFormat,
                                               std::chrono::microseconds timestamp) {
  std::unique_lock<std::mutex>(m_mutex);

  if (m_stereoDemuxer == nullptr && stereoDemuxerRequired() || stereoFormatChanged()) {
    updateStereoDemuxer(frameFormat);
  }

  auto frameSize = frameFormat.frameSize;

  // should demux stereo
  if (m_stereoDemuxer != nullptr && not data.empty()) {
    m_stereoDemuxer->setSize(frameFormat.frameSize);
    m_stereoDemuxer->setPixelFormat(frameFormat.pixelFormat);
    frameSize = m_stereoDemuxer->demuxedSize();

    std::vector<uint8_t> dataLeft, dataRight;
    std::tie(dataLeft, dataRight) = m_stereoDemuxer->demux(
        std::vector<uint8_t>(data[0].data(), data[0].data() + data[0].size()));

    m_images[0] =
        QImage(dataLeft.data(), frameSize.getWidth(), frameSize.getHeight(), QImage::Format_ARGB32)
            .copy();
    m_images[1] =
        QImage(dataRight.data(), frameSize.getWidth(), frameSize.getHeight(), QImage::Format_ARGB32)
            .copy();
  }
  // input in separate images
  else if (data.size() > 1) {
    m_images[0] =
        QImage(data[0].data(), frameSize.getWidth(), frameSize.getHeight(), QImage::Format_ARGB32)
            .copy();
    m_images[1] =
        QImage(data[1].data(), frameSize.getWidth(), frameSize.getHeight(), QImage::Format_ARGB32)
            .copy();
  }
  // no input.. outputing black image
  else {
    m_images[0] =
        QImage(frameSize.getWidth(), frameSize.getHeight(), QImage::Format::Format_ARGB32);
    m_images[1] =
        QImage(frameSize.getWidth(), frameSize.getHeight(), QImage::Format::Format_ARGB32);
    m_images[0].fill(Qt::black);
    m_images[1].fill(Qt::black);
  }

  m_timestamp = timestamp;
  m_imagesDirty = true;
}

void VideoSynchronizer::checkForIncomingImage() {
  std::unique_lock<std::mutex> lock(m_mutex);
  if (m_imagesDirty) {
    // should use Qt::QueuedConnection for minimal time spent in timer callback
    emit incomingImagePair(m_images[0], m_images[1], m_timestamp);
    m_imagesDirty = false;
  }
}

void VideoSynchronizer::setStereoVideoFormat(s3d::Stereo3DFormat format) {
  m_stereoFormat = format;
}

void VideoSynchronizer::setLeftFilename(std::string filename) {
  m_leftFilename = std::move(filename);
  m_leftFileReady = true;
}

void VideoSynchronizer::setRightFilename(std::string filename) {
  m_rightFilename = std::move(filename);
  m_rightFileReady = true;
}

void VideoSynchronizer::loadStereoVideo() {
  loadStereoVideo(m_leftFilename, m_rightFilename, m_stereoFormat);
}

void VideoSynchronizer::loadStereoVideo(const std::string& leftFile,
                                        const std::string& rightFile,
                                        s3d::Stereo3DFormat stereoFormat) {
  // reset file state
  m_leftFileReady = false;
  m_rightFileReady = false;

  m_videoLoaded = true;

  s3d::VideoFileParserFFmpeg parser(leftFile);
  s3d::VideoCaptureFormat format;
  if (not parser.Initialize(&format)) {
    m_videoLoaded = false;
    return;
  }

  m_videoDuration = parser.VideoDuration();
  m_stereoDemuxer =
      s3d::StereoDemuxerFactoryCV{}.create(stereoFormat, format.frameSize, format.pixelFormat);
  m_images.resize(2);

  if (stereoFormat == s3d::Stereo3DFormat::Separate) {
    m_videoCaptureDevice =
        std::make_unique<s3d::FileVideoCaptureDevice3D>(leftFile + ";" + rightFile);
  } else {
    m_videoCaptureDevice = std::make_unique<s3d::FileVideoCaptureDeviceFFmpeg>(leftFile);
  }

  m_timer = createAndStartTimer();
  s3d::VideoCaptureFormat suggestedFormat{{}, -1, s3d::VideoPixelFormat::BGRA};
  m_videoCaptureDevice->AllocateAndStart(suggestedFormat, this);

  // start paused
  m_videoCaptureDevice->MaybeSuspend();
  m_videoCaptureDevice->RequestRefreshFrame();
}

std::unique_ptr<QTimer> VideoSynchronizer::createAndStartTimer() {
  auto timer = std::make_unique<QTimer>(this);
  timer->setTimerType(Qt::PreciseTimer);
  connect(timer.get(), SIGNAL(timeout()), this, SLOT(checkForIncomingImage()));
  timer->start(10);
  return timer;
}

bool VideoSynchronizer::isVideoLoaded() {
  return m_videoLoaded;
}

bool VideoSynchronizer::stereoFormatChanged() {
  return m_stereoDemuxer != nullptr && m_stereoDemuxer->getStereoFormat() != m_stereoFormat;
}

void VideoSynchronizer::updateStereoDemuxer(const s3d::VideoCaptureFormat& format) {
  m_stereoDemuxer =
      m_stereoDemuxerFactory->create(m_stereoFormat, format.frameSize, format.pixelFormat);
}

bool VideoSynchronizer::stereoDemuxerRequired() {
  return m_stereoFormat != s3d::Stereo3DFormat::Separate;
}

bool VideoSynchronizer::isVideoReadyToLoad() {
  if (m_stereoFormat == s3d::Stereo3DFormat::Separate) {
    return m_leftFileReady && m_rightFileReady;
  }
  return m_leftFileReady;
}
