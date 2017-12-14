#include "s3d/video/file_parser/ffmpeg/scaler.h"

namespace s3d {

Scaler::Scaler(AVCodecContext* codecContext, enum AVPixelFormat dstFormat)
    : codecContext_(codecContext) {
  // get this from codec context
  int srcWidth{codecContext->width}, dstWidth{codecContext->width};
  int srcHeight{codecContext->height}, dstHeight{codecContext->height};
  AVPixelFormat srcFormat{codecContext->pix_fmt};

  swsContext_ = ffmpeg::UniquePtr<SwsContext>(sws_getContext(srcWidth,
                                                             srcHeight,
                                                             srcFormat,
                                                             dstWidth,
                                                             dstHeight,
                                                             dstFormat,
                                                             SWS_BILINEAR,
                                                             nullptr,
                                                             nullptr,
                                                             nullptr));

  // buffer is going to be written to rawvideo file, no alignment
  dstBufferSize_ = ffmpeg::imgutils::image_alloc(
      dstBuffer_, dstBufferLineSize_, dstWidth, dstHeight, dstFormat, 1);

  // transfer ownership for automatic free
  dstBufferPtr_ = ffmpeg::UniquePtr<uint8_t>(dstBuffer_[0]);
}

void Scaler::scaleFrame(AVFrame* frame, std::vector<uint8_t>* out) {
  // need to reallocate a frame for different frame properties
  assert(frame->height == codecContext_->height && frame->width == codecContext_->width &&
         frame->format == codecContext_->pix_fmt);

  // convert to destination format
  sws_scale(swsContext_.get(),
            const_cast<const uint8_t* const*>(frame->data),
            frame->linesize,
            0,
            frame->height,
            dstBuffer_,
            dstBufferLineSize_);

  out->resize(dstBufferSize_);
  std::copy(dstBufferPtr_.get(), dstBufferPtr_.get() + dstBufferSize_, std::begin(*out));
}

}  // namespace s3d
