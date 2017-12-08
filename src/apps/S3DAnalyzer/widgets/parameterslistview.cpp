#include "parameterslistview.h"

#include <QPushButton>

ParametersListView::ParametersListView(QWidget* parent) : QWidget(parent) {
  generateParameterWidgets({{"Roll", "°"},
                            {"Vertical", "°"},
                            {"Pan Keystone", "°/m"},
                            {"Tilt Keystone", "°/m"},
                            {"Tilt Offset", "%"},
                            {"Zoom", "%"}});
  setParameter("Roll", 0.1);
  setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
}

ParametersListView::~ParametersListView() {
  clearParameters();
}

void ParametersListView::updateUI() {
  // update layout
  update();
}

void ParametersListView::generateParameterWidgets(std::vector<LabelSymbolPair> labels) {
  auto layout = std::make_unique<QHBoxLayout>();
  for (auto& label : labels) {
    auto paramWidget = std::make_unique<ParameterWidget>(label.first, label.second);
    layout->addWidget(paramWidget.get());
    m_parameters.insert(
        std::pair<QString, std::unique_ptr<ParameterWidget>>(label.first, std::move(paramWidget)));
  }

  // give up layout ownership
  m_layout = layout.get();
  setLayout(layout.release());
}

void ParametersListView::setParameter(const QString& label, float value) {
  // check if parameter is in map
  auto it = m_parameters.find(label);

  // if in map, set value
  if (it != m_parameters.end()) {
    it->second->setValue(value);
  }
}

void ParametersListView::clearParameters() {
  QLayoutItem* child;
  while ((child = m_layout->takeAt(0)) != nullptr) {
    m_layout->removeItem(child);
    delete child;
  }

  // deletes ParameterWidgets automatically with unique_ptr
  m_parameters.clear();
}
