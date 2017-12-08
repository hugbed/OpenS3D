#ifndef CONTROLLERS_UPDATERECTIFICATIONCONTROLLER_H
#define CONTROLLERS_UPDATERECTIFICATIONCONTROLLER_H

#include "imageoperationactioncontroller.h"

#include "s3d/cv/image_operation/image_operation.h"

#include <QAction>

#include <gsl/gsl>

class ImageOperationTriggerController : public ImageOperationActionController {
Q_OBJECT

public:
  ImageOperationTriggerController(gsl::not_null<QAction*> action,
                                  gsl::not_null<s3d::image_operation::ImageOperation*> imageOperation);

  void onActionTriggered() override;
};

#endif //CONTROLLERS_UPDATERECTIFICATIONCONTROLLER_H
