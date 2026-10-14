/*
 * TetgenWidget.cpp
 *
 * This widget provides options for the Tetgen method.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include <QFont>
#include <TetgenWidget.hpp>


TetgenWidget::TetgenWidget(QWidget *parent) : TetraMethodWidget(parent) {
  labelTitle.setText("Tetgen");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);

  qualityBoundsLabel.setText("Quality Bound");
  qualityBoundsLabel.setFont(boldFont);
  qualityBoundsSpinBox.setValue(2.0);
  qualityBoundsSpinBox.setSingleStep(0.1);
  qualityBoundsSpinBox.setMinimum(1.0);
  qualityBoundsLabel.setMaximumHeight(20);
  qualityBoundsSpinBox.setMaximumHeight(20);
  qualityBoundsLayout.addWidget(&qualityBoundsLabel);
  qualityBoundsLayout.addWidget(&qualityBoundsSpinBox);

  cellSizeLabel.setText("Cell Size (0=auto)");
  cellSizeLabel.setFont(boldFont);
  cellSizeSpinBox.setValue(0.0);
  cellSizeSpinBox.setDecimals(4);
  cellSizeSpinBox.setMaximum(1000000.0);
  cellSizeSpinBox.setSingleStep(0.01);
  cellSizeSpinBox.setMinimum(0.0);
  cellSizeLabel.setMaximumHeight(20);
  cellSizeSpinBox.setMaximumHeight(20);
  cellSizeLayout.addWidget(&cellSizeLabel);
  cellSizeLayout.addWidget(&cellSizeSpinBox);

  usePLCCheckBox.setText("Use PLC");
  usePLCCheckBox.setChecked(true);

  generateTetrahedraButton.setText("Generate Tetrahedra");

  layout.addWidget(&labelTitle);
  layout.addLayout(&qualityBoundsLayout);
  layout.addLayout(&cellSizeLayout);
  layout.addWidget(&usePLCCheckBox);
  layout.addWidget(&generateTetrahedraButton);
  layout.addStretch();
}

QVariantMap TetgenWidget::parameters() const {
  QVariantMap p;
  p["qualityBound"] = qualityBoundsSpinBox.value();
  p["cellSize"] = cellSizeSpinBox.value();
  p["usePLC"] = usePLCCheckBox.isChecked();
  return p;
}
