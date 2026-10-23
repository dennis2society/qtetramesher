/*
 * FTetWildWidget.cpp
 *
 * This widget provides options for the fTetWild tetrahedralization method.
 */

#include <FTetWildWidget.hpp>
#include <QFont>

FTetWildWidget::FTetWildWidget(QWidget *parent) : TetraMethodWidget(parent) {
  labelTitle.setText("fTetWild Tetrahedralize");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);

  edgeLengthLabel.setText("Edge Length (rel.)");
  edgeLengthLabel.setFont(boldFont);
  edgeLengthLabel.setToolTip(
      "Ideal edge length relative to the bounding box diagonal (-l)");
  edgeLengthSpinBox.setRange(0.001, 1.0);
  edgeLengthSpinBox.setDecimals(4);
  edgeLengthSpinBox.setSingleStep(0.005);
  edgeLengthSpinBox.setValue(0.05);
  edgeLengthLabel.setMaximumHeight(20);
  edgeLengthSpinBox.setMaximumHeight(20);
  edgeLengthLayout.addWidget(&edgeLengthLabel);
  edgeLengthLayout.addWidget(&edgeLengthSpinBox);

  epsilonLabel.setText("Envelope (rel.)");
  epsilonLabel.setFont(boldFont);
  epsilonLabel.setToolTip(
      "Allowed distance of the result from the input surface, relative to "
      "the bounding box diagonal (-e). Larger values are faster but less "
      "accurate.");
  epsilonSpinBox.setRange(0.00001, 0.1);
  epsilonSpinBox.setDecimals(5);
  epsilonSpinBox.setSingleStep(0.0005);
  epsilonSpinBox.setValue(0.001);
  epsilonLabel.setMaximumHeight(20);
  epsilonSpinBox.setMaximumHeight(20);
  epsilonLayout.addWidget(&epsilonLabel);
  epsilonLayout.addWidget(&epsilonSpinBox);

  stopEnergyLabel.setText("Stop Energy");
  stopEnergyLabel.setFont(boldFont);
  stopEnergyLabel.setToolTip(
      "The optimization stops when the maximum tetrahedron energy is below "
      "this value (--stop-energy). Smaller values give better tetrahedra but "
      "take longer.");
  stopEnergySpinBox.setRange(3.0, 1000.0);
  stopEnergySpinBox.setDecimals(1);
  stopEnergySpinBox.setValue(10.0);
  stopEnergyLabel.setMaximumHeight(20);
  stopEnergySpinBox.setMaximumHeight(20);
  stopEnergyLayout.addWidget(&stopEnergyLabel);
  stopEnergyLayout.addWidget(&stopEnergySpinBox);

  coarsenCheckBox.setText("Coarsen");
  coarsenCheckBox.setToolTip("Coarsen the output as much as possible");
  coarsenCheckBox.setMaximumHeight(20);

  generateTetrahedraButton.setText("Generate Tetrahedra");

  layout.addWidget(&labelTitle);
  layout.addLayout(&edgeLengthLayout);
  layout.addLayout(&epsilonLayout);
  layout.addLayout(&stopEnergyLayout);
  layout.addWidget(&coarsenCheckBox);
  layout.addWidget(&generateTetrahedraButton);
}

QVariantMap FTetWildWidget::parameters() const {
  QVariantMap p;
  p["edgeLengthRel"] = edgeLengthSpinBox.value();
  p["epsilonRel"] = epsilonSpinBox.value();
  p["stopEnergy"] = stopEnergySpinBox.value();
  p["coarsen"] = coarsenCheckBox.isChecked();
  return p;
}
