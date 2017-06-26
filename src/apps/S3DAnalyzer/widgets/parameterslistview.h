#ifndef WIDGETS_ALIGNMENTPARAMETERSWIDGET_H
#define WIDGETS_ALIGNMENTPARAMETERSWIDGET_H

#include <QHBoxLayout>
#include <QWidget>
#include <QString>

#include "parameterwidget.h"

#include <memory>

class ParametersListView : public QWidget {
  Q_OBJECT
 public:
  using LabelSymbolPair = std::pair<QString, QString>;

  explicit ParametersListView(QWidget* parent = nullptr);
  ~ParametersListView() override;
  void updateUI();

 signals:

 public slots:
  void generateParameterWidgets(std::vector<LabelSymbolPair> labels);
  void setParameter(const QString& label, float value);
  void clearParameters();

 private:
  std::map<QString, std::unique_ptr<ParameterWidget>> m_parameters;
  QHBoxLayout* m_layout;
};

#endif  // WIDGETS_ALIGNMENTPARAMETERSWIDGET_H
