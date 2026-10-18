/*
 * GenericIsosurfaceStuffingWidget.cpp
 *
 * This widget provides the UI options for the Generic Isosurface Stuffing
 * implementation (Shewchuk's algorithm).
 *
 *  Created 2026
 */

#include "GenericIsosurfaceStuffingWidget.hpp"

#include <QFont>


GenericIsosurfaceStuffingWidget::GenericIsosurfaceStuffingWidget(QWidget *parent)
    : TetraMethodWidget(parent) {
  labelTitle.setText("Generic Isosurface Stuffing");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);
  cellSizeLabel.setText("CellSize");
  cellSizeLabel.setFont(boldFont);
  cellSizeSpinBox.setValue(1.0);
  cellSizeSpinBox.setSingleStep(0.01);
  cellSizeLabel.setMaximumHeight(20);
  cellSizeSpinBox.setMaximumHeight(20);
  cellSizeLayout.addWidget(&cellSizeLabel);
  cellSizeLayout.addWidget(&cellSizeSpinBox);
  alphaShortLabel.setText("AlphaShort");
  // Labelle & Shewchuk's proven thresholds; larger values warp lattice points
  // so far that the stencils degenerate into slivers.
  alphaShortSpinBox.setDecimals(4);
  alphaShortSpinBox.setRange(0.0, 0.49);
  alphaShortSpinBox.setValue(0.3548);
  alphaShortSpinBox.setSingleStep(0.01);
  alphaLongLabel.setText("AlphaLong");
  alphaLongSpinBox.setDecimals(4);
  alphaLongSpinBox.setRange(0.0, 0.49);
  alphaLongSpinBox.setValue(0.2151);
  alphaLongSpinBox.setSingleStep(0.01);
  alphaShortLabel.setMaximumHeight(20);
  alphaShortSpinBox.setMaximumHeight(20);
  alphaLongLabel.setMaximumHeight(20);
  alphaLongSpinBox.setMaximumHeight(20);
  alphaLayout.addWidget(&alphaShortLabel);
  alphaLayout.addWidget(&alphaShortSpinBox);
  alphaLayout.addWidget(&alphaLongLabel);
  alphaLayout.addWidget(&alphaLongSpinBox);
  gradedCheckBox.setText("Graded (large interior tets)");
  gradedCheckBox.setChecked(false);
  generateTetrahedraButton.setText("Generate Tetrahedra");
  layout.addWidget(&labelTitle);
  layout.addLayout(&cellSizeLayout);
  layout.addLayout(&alphaLayout);
  layout.addWidget(&gradedCheckBox);
  layout.addWidget(&generateTetrahedraButton);
  layout.addStretch();
}

QVariantMap GenericIsosurfaceStuffingWidget::parameters() const {
  QVariantMap p;
  p["cellSize"] = cellSizeSpinBox.value();
  p["alphaShort"] = alphaShortSpinBox.value();
  p["alphaLong"] = alphaLongSpinBox.value();
  p["graded"] = gradedCheckBox.isChecked();
  return p;
}
