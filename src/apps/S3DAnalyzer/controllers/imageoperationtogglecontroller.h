#ifndef CONTROLLERS_IMAGEOPERATIONTOGGLECONTROLLER_H
#define CONTROLLERS_IMAGEOPERATIONTOGGLECONTROLLER_H

#include "imageoperationactioncontroller.h"

#include "s3d/cv/image_operation/image_operation.h"

#include <QAction>

#include <gsl/gsl>

class ImageOperationToggleController : public ImageOperationActionController {
  Q_OBJECT

public:
  ImageOperationToggleController(gsl::not_null<QAction*> action,
                                 gsl::not_null<s3d::image_operation::ImageOperation*> imageOperation);

  void onActionTriggered() override;
};

#endif //CONTROLLERS_IMAGEOPERATIONTOGGLECONTROLLER_H
