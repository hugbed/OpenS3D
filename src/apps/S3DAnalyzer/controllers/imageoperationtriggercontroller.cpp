#include "imageoperationtriggercontroller.h"

using ImageOperation = s3d::image_operation::ImageOperation;

ImageOperationTriggerController::ImageOperationTriggerController(gsl::not_null<QAction*> action,
                                                                 gsl::not_null<ImageOperation*> imageOperation)
  : ImageOperationActionController(action, imageOperation) {}

void ImageOperationTriggerController::onActionTriggered() {
  bool wasEnabled = m_imageOperation->isEnabled();
  m_imageOperation->enable();

  emit imageOperationTriggered();

  if (wasEnabled) {
    m_imageOperation->enable();
  } else {
    m_imageOperation->disable();
  }
}
