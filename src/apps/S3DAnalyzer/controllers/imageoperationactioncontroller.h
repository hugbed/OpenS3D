#ifndef CONTROLLERS_IMAGEOPERATIONACTIONCONTROLLER_H
#define CONTROLLERS_IMAGEOPERATIONACTIONCONTROLLER_H

#include <QObject>

#include "worker/image_operation.h"

#include <QAction>

#include <gsl/gsl>

class ImageOperationActionController : public QObject {
  Q_OBJECT

public:
  ImageOperationActionController(gsl::not_null<QAction*> action,
                                 gsl::not_null<ImageOperation*> imageOperation);

  void updateImageOperationFromActionState();

signals:
  void imageOperationToggled();

private:
  QAction* m_action;
  ImageOperation* m_imageOperation;
};

#endif //CONTROLLERS_IMAGEOPERATIONACTIONCONTROLLER_H
