#include "imageoperationtogglecontroller.h"

using ImageOperation = s3d::image_operation::ImageOperation;

ImageOperationToggleController::ImageOperationToggleController(gsl::not_null<QAction*> action,
                                                               gsl::not_null<ImageOperation*> imageOperation)
        : ImageOperationActionController(action, imageOperation) {}

void ImageOperationToggleController::onActionTriggered() {
  if (m_action->isChecked()) {
    m_imageOperation->enable();
  } else {
    m_imageOperation->disable();
  }
  emit ImageOperationActionController::imageOperationTriggered();
}
