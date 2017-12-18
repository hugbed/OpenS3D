#include "featuressettingsdialog.h"

#include <s3d/cv/features/match_finder_surf.h>

// static
std::unique_ptr<s3d::MatchFinderCV> MatchFinderFromIndexFactory::create(int index) {
  switch (index) {
    default:
      return std::make_unique<s3d::MatchFinderCV>();
    case 1:
      return std::make_unique<s3d::MatchFinderSurf>();
  }
}

FeaturesSettingsDialog::FeaturesSettingsDialog(QWidget* parent)
: QDialog(parent),
  ui(std::make_unique<Ui::FeaturesSettingsDialog>())
{
  ui->setupUi(this);

  connect(ui->buttonBox, &QDialogButtonBox::accepted, [this] {
    hide();
  });

  connect(ui->imageScalingSpinBox,
          static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
          [this](int newValue) {
     emit imageScalingRatioChanged(static_cast<float>(newValue) / 100.0f);
   });

  connect(ui->maxNbFeaturesSpinBox,
          static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
          [this](int newValue) {
    emit maxNbFeaturesChanged(newValue);
  });

  connect(ui->featureDetectorComboBox,
          static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          [this](int newIndex) {
    emit featureDetectorChanged(newIndex);
  });
}
