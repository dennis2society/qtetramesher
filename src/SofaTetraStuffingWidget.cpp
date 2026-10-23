/*
 * SofaTetraStuffingWidget.cpp
 *
 * This widget provides the UI options for the SOFA TetraStuffing
 * implementation.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include "SofaTetraStuffingWidget.hpp"

#include <QFont>


SofaTetraStuffingWidget::SofaTetraStuffingWidget(QWidget *parent)
    : TetraMethodWidget(parent) {
  labelTitle.setText("SOFA TetraStuffing");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);
  tetraSizeLabel.setText("TetraSize");
  tetraSizeLabel.setFont(boldFont);
  tetraSizeSpinBox.setValue(1.0);
  tetraSizeSpinBox.setSingleStep(0.01);
  tetraSizeLabel.setMaximumHeight(20);
  tetraSizeSpinBox.setMaximumHeight(20);
  tetraSizeLayout.addWidget(&tetraSizeLabel);
  tetraSizeLayout.addWidget(&tetraSizeSpinBox);
  alphaShortLabel.setText("AlphaShort");
  alphaShortSpinBox.setValue(0.43);
  alphaShortSpinBox.setSingleStep(0.01);
  alphaLongLabel.setText("AlphaLong");
  alphaLongSpinBox.setValue(0.25);
  alphaLongSpinBox.setSingleStep(0.01);
  alphaShortLabel.setMaximumHeight(20);
  alphaShortSpinBox.setMaximumHeight(20);
  alphaLongLabel.setMaximumHeight(20);
  alphaLongSpinBox.setMaximumHeight(20);
  alphaOptionsLayout.addWidget(&alphaShortLabel);
  alphaOptionsLayout.addWidget(&alphaShortSpinBox);
  alphaOptionsLayout.addWidget(&alphaLongLabel);
  alphaOptionsLayout.addWidget(&alphaLongSpinBox);
  splitTetrasCheckBox.setText("Split Tetras");
  splitTetrasCheckBox.setChecked(true);
  snapToPointsCheckBox.setText("Snap To Points");
  snapToPointsCheckBox.setChecked(true);
  additionalOptionsLayout.addWidget(&splitTetrasCheckBox);
  additionalOptionsLayout.addWidget(&snapToPointsCheckBox);
  generateTetrahedraButton.setText("Generate Tetrahedra");
  layout.addWidget(&labelTitle);
  layout.addLayout(&tetraSizeLayout);
  layout.addLayout(&alphaOptionsLayout);
  layout.addLayout(&additionalOptionsLayout);
  layout.addWidget(&generateTetrahedraButton);
}

QVariantMap SofaTetraStuffingWidget::parameters() const {
  QVariantMap p;
  p["size"] = tetraSizeSpinBox.value();
  p["alphaShort"] = alphaShortSpinBox.value();
  p["alphaLong"] = alphaLongSpinBox.value();
  p["snapToPoints"] = snapToPointsCheckBox.isChecked();
  p["splitTetras"] = splitTetrasCheckBox.isChecked();
  return p;
}
