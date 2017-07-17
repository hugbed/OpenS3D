#ifndef UTILITIES_DEPTHINTENSITYGRADIENT_H
#define UTILITIES_DEPTHINTENSITYGRADIENT_H

#include <QLinearGradient>

class DepthIntensityGradient : public QLinearGradient {
 public:
  DepthIntensityGradient(const QPointF& topLeft, const QPointF& bottomRight);

 public:
  // todo: optimization: compute the right step, then mix only two colors?
  QColor colorAt(float value) {
    if (value >= 1.0f) {
      return colors[5];
    }
    if (value <= 0.0f) {
      return colors[0];
    }

    QColor color = mix(colors[0], colors[1], smoothstep(steps[0], steps[1], value));
    color = mix(color, colors[2], smoothstep(steps[1], steps[2], value));
    color = mix(color, colors[3], smoothstep(steps[2], steps[3], value));
    color = mix(color, colors[4], smoothstep(steps[3], steps[4], value));
    color = mix(color, colors[5], smoothstep(steps[4], steps[5], value));
    return color;
  }

  QColor mix(const QColor& color1, const QColor& color2, float a) {
    auto r = mix(color1.redF(), color2.redF(), a);
    auto g = mix(color1.greenF(), color2.greenF(), a);
    auto b = mix(color1.blueF(), color2.blueF(), a);
    return QColor(r * 255.0f, g * 255.0f, b * 255.0f);
  }

 private:
  float smoothstep(float edge0, float edge1, float x) {
    float t = qBound((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
  }

  float mix(float x, float y, float a) { return x * (1 - a) + y * a; }

 public:
  std::vector<QColor> colors;
  std::vector<float> steps;

  static const QColor red;
  static const QColor orange;
  static const QColor green;
  static const QColor blue;
  static const QColor purple;
};

#endif  // UTILITIES_DEPTHINTENSITYGRADIENT_H
