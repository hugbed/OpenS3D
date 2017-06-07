#ifndef S3D_ANALYZER_WORKER_DEPTHANALYZER_H
#define S3D_ANALYZER_WORKER_DEPTHANALYZER_H

#include <QImage>
#include <QVector2D>
#include <QColor>

class DepthAnalyzer {
 public:
  void analyze(const QImage& imageLeft, const QImage& imageRight);

  double minDisparity{};
  double maxDisparity{};

  double roll{};
  double vertical{};
  double panKeystone{};
  double tiltKeystone{};
  double tiltOffset{};
  double zoom{};

  std::vector<QVector2D> featurePoints;
  std::vector<float> disparitiesPercent;
};

#endif  // S3D_ANALYZER_WORKER_DEPTHANALYZER_H
