#include <QtTest/QtTest>

#include "utilities/depthintensitygradient.h"

class TestDepthIntensityGradient : public QObject {
  Q_OBJECT
 private slots:
  void colorAtMiddle();
  void colorAtMin();
  void colorAtMax();
  void colorBeforeMin();
  void colorAfterMax();

  // color before min, after min
};

// assuming zmin = -1, zmax = 3, screenWidth = 1.0f
void TestDepthIntensityGradient::colorAtMiddle() {
  DepthIntensityGradient g({}, {});
  auto res = g.colorAt(0.05);
  QCOMPARE(res, g.mix(DepthIntensityGradient::red, DepthIntensityGradient::orange, 0.5));
}

void TestDepthIntensityGradient::colorAtMin() {
  DepthIntensityGradient g({}, {});
  auto res = g.colorAt(0.0);
  QCOMPARE(res, DepthIntensityGradient::red);
}

void TestDepthIntensityGradient::colorAtMax() {
  DepthIntensityGradient g({}, {});
  auto res = g.colorAt(1.0);
  QCOMPARE(res, DepthIntensityGradient::purple);
}

void TestDepthIntensityGradient::colorBeforeMin() {
  DepthIntensityGradient g({}, {});
  auto res = g.colorAt(-1.0f);
  QCOMPARE(res, DepthIntensityGradient::red);
}

void TestDepthIntensityGradient::colorAfterMax() {
  DepthIntensityGradient g({}, {});
  auto res = g.colorAt(1.5f);
  QCOMPARE(res, DepthIntensityGradient::purple);
}

QTEST_MAIN(TestDepthIntensityGradient)
#include "tests_depthintensitygradient.moc"
