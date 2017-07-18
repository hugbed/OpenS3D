#include <QtTest/QtTest>

#include "rendering/entity/stereo/viewercentricentity.h"

#include <s3d/disparity/viewer_context.h>

class TestViewerCentricEntity : public QObject {
  Q_OBJECT
 private slots:
  void centerInMiddleVerticalLeftHorizontal();
  void pointInsideWidgetRightPlace();
};

// assuming zmin = -1, zmax = 3, screenWidth = 1.0f
void TestViewerCentricEntity::centerInMiddleVerticalLeftHorizontal() {
  ViewerContext context{2.0, 0.05, 1.0f, 1920};
  ViewerCentricEntity vce;
  vce.setViewerContext(&context);
  vce.setDepthRangeMeters(-1.0f, 3.0f);
  auto res = vce.worldToWidget({0, 0}, 200, 100);
  QCOMPARE(res, QPoint(0, 25));
}

void TestViewerCentricEntity::pointInsideWidgetRightPlace() {
  ViewerContext context{2.0, 0.05, 1.0f, 1920};
  ViewerCentricEntity vce;
  vce.setViewerContext(&context);
  vce.setDepthRangeMeters(-1.0f, 3.0f);
  auto res = vce.worldToWidget({0.5, 2.0}, 200, 100);
  QCOMPARE(res, QPoint(100, 75));
}

QTEST_MAIN(TestViewerCentricEntity)
#include "tests_viewercentricentity.moc"
