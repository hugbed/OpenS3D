#include "imageoperationactioncontroller.h"

using ImageOperation = s3d::image_operation::ImageOperation;

ImageOperationActionController::ImageOperationActionController(gsl::not_null<QAction*> action,
                                                               gsl::not_null<ImageOperation*> imageOperation)
        : m_action{action}
        , m_imageOperation{imageOperation}
{
  connect(m_action, &QAction::triggered, [this] { updateImageOperationFromActionState(); });
}

void ImageOperationActionController::updateImageOperationFromActionState() {
  if (m_action->isChecked()) {
    m_imageOperation->enable();
  } else {
    m_imageOperation->disable();
  }
  emit imageOperationToggled();
}
