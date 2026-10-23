/*
 * GMSHWidget.cpp
 *
 * This widget provides options for the GMSH tetrahedralization method.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include <QFont>
#include <GMSHWidget.hpp>


GMSHWidget::GMSHWidget(QWidget *parent) : TetraMethodWidget(parent) {
  labelTitle.setText("GMSH Tetrahedralize");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);

  meshSizeMinLabel.setText("Mesh Size Min (0=auto)");
  meshSizeMinLabel.setFont(boldFont);
  meshSizeMinSpinBox.setValue(0.0);
  meshSizeMinSpinBox.setSingleStep(0.01);
  meshSizeMinSpinBox.setMinimum(0.0);
  meshSizeMinSpinBox.setMaximum(1000000.0);
  meshSizeMinSpinBox.setDecimals(4);
  meshSizeMinLabel.setMaximumHeight(20);
  meshSizeMinSpinBox.setMaximumHeight(20);
  meshSizeMinLayout.addWidget(&meshSizeMinLabel);
  meshSizeMinLayout.addWidget(&meshSizeMinSpinBox);

  meshSizeMaxLabel.setText("Mesh Size Max (0=auto)");
  meshSizeMaxLabel.setFont(boldFont);
  meshSizeMaxSpinBox.setValue(0.0);
  meshSizeMaxSpinBox.setSingleStep(0.01);
  meshSizeMaxSpinBox.setMinimum(0.0);
  meshSizeMaxSpinBox.setMaximum(1000000.0);
  meshSizeMaxSpinBox.setDecimals(4);
  meshSizeMaxLabel.setMaximumHeight(20);
  meshSizeMaxSpinBox.setMaximumHeight(20);
  meshSizeMaxLayout.addWidget(&meshSizeMaxLabel);
  meshSizeMaxLayout.addWidget(&meshSizeMaxSpinBox);

  algorithmLabel.setText("Algorithm");
  algorithmLabel.setFont(boldFont);
  algorithmComboBox.addItem("Delaunay (1)");
  algorithmComboBox.addItem("HXT (10)");
  algorithmLabel.setMaximumHeight(20);
  algorithmComboBox.setMaximumHeight(24);
  algorithmLayout.addWidget(&algorithmLabel);
  algorithmLayout.addWidget(&algorithmComboBox);

  generateTetrahedraButton.setText("Generate Tetrahedra");

  layout.addWidget(&labelTitle);
  layout.addLayout(&meshSizeMinLayout);
  layout.addLayout(&meshSizeMaxLayout);
  layout.addLayout(&algorithmLayout);
  layout.addWidget(&generateTetrahedraButton);
}

QVariantMap GMSHWidget::parameters() const {
  // Map combo-box index to GMSH algorithm number. Frontal (4) is not offered,
  // it requires a GMSH build with Netgen.
  static const int algorithmMap[] = {1, 10};
  const int algoIdx = algorithmComboBox.currentIndex();
  QVariantMap p;
  p["meshSizeMin"] = meshSizeMinSpinBox.value();
  p["meshSizeMax"] = meshSizeMaxSpinBox.value();
  p["algorithm3D"] =
      (algoIdx >= 0 && algoIdx < 2) ? algorithmMap[algoIdx] : algorithmMap[0];
  return p;
}

void GMSHWidget::applyUsedParameters(const QVariantMap &used) {
  if (meshSizeMinSpinBox.value() <= 0.0 && used.contains("meshSizeMin"))
    meshSizeMinSpinBox.setValue(used.value("meshSizeMin").toDouble());
  if (meshSizeMaxSpinBox.value() <= 0.0 && used.contains("meshSizeMax"))
    meshSizeMaxSpinBox.setValue(used.value("meshSizeMax").toDouble());
}
