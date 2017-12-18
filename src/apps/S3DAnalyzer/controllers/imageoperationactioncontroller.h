#ifndef CONTROLLERS_IMAGEOPERATIONACTIONCONTROLLER_H
#define CONTROLLERS_IMAGEOPERATIONACTIONCONTROLLER_H

#include <QObject>

#include "s3d/cv/image_operation/image_operation.h"

#include <QAction>

#include <gsl/gsl>

class ImageOperationActionController : public QObject {
  Q_OBJECT

public:
  ImageOperationActionController(gsl::not_null<QAction*> action,
                                 gsl::not_null<s3d::image_operation::ImageOperation*> imageOperation);

  virtual void onActionTriggered() = 0;

signals:
  void imageOperationTriggered();

protected:
  QAction* m_action;
  s3d::image_operation::ImageOperation* m_imageOperation;
};

#endif //CONTROLLERS_IMAGEOPERATIONACTIONCONTROLLER_H
