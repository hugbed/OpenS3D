#include "depthintensitygradient.h"

DepthIntensityGradient::DepthIntensityGradient(const QPointF& topLeft, const QPointF& bottomRight)
    : QLinearGradient(topLeft, bottomRight),
      colors{red, orange, green, green, blue, purple},
      steps{0.0f, 0.1f, 0.2f, 0.8f, 0.9f, 1.0f} {
  for (int i = 0; i < colors.size(); ++i) {
    setColorAt(steps[i], colors[i]);
  }
}

const QColor DepthIntensityGradient::red = QColor{214, 69, 65};
const QColor DepthIntensityGradient::orange = QColor{235, 151, 78};
const QColor DepthIntensityGradient::green = QColor{135, 211, 124};
const QColor DepthIntensityGradient::blue = QColor{68, 108, 179};
const QColor DepthIntensityGradient::purple = QColor{102, 51, 153};
