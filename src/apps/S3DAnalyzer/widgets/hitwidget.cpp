#include "hitwidget.h"

#include "lineeditnocursor.h"

#include <memory>

HITWidget::HITWidget(QWidget *parent)
    : QDoubleSpinBox(parent)
{
    setRange(-3.0, 6.0);
    setSingleStep(0.1);

    auto lineEdit = std::make_unique<LineEditNoCursor>();
    lineEdit->setAlignment(Qt::AlignCenter);
    setLineEdit(lineEdit.release());

    setMinimumSize(width() + 5, height() + 5);
    setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));

    QString spinBoxStyle(
        "QDoubleSpinBox"
        "{"
        "   border-width: 0px;"
        "   selection-background-color: #31363b;"
        "}"
        "QDoubleSpinBox:hover"
        "{"
        "    border-width: 0px;"
        "    selection-background-color: #31363b;"
        "}"
        //"QDoubleSpinBox::up-button {"
        //"    image: url(:/images/spinup.png);"
        //"}"
   ); // to change up button image (for red, orange, green).

    setStyleSheet(spinBoxStyle);
}
