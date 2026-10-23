/*
 * CGALTetrahedralizeWidget.cpp
 *
 * This widget provides options for the CGAL delaunay triangulation.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include "CGALTetrahedralizeWidget.hpp"

#include <QFont>


CGALTetrahedralizeWidget::CGALTetrahedralizeWidget(QWidget *parent)
    : TetraMethodWidget(parent) {
  labelTitle.setText("CGAL Delaunay Triangulation");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);
  cellSizeLabel.setText("CellSize");
  cellSizeLabel.setFont(boldFont);
  cellSizeSpinBox.setValue(0.5);
  cellSizeSpinBox.setSingleStep(0.01);
  cellSizeLabel.setMaximumHeight(20);
  cellSizeSpinBox.setMaximumHeight(20);
  cellSizeLayout.addWidget(&cellSizeLabel);
  cellSizeLayout.addWidget(&cellSizeSpinBox);
  facetAngleLabel.setText("Facet Angle");
  facetAngleSpinBox.setValue(25.0);
  facetAngleSpinBox.setSingleStep(0.05);
  facetSizeLabel.setText("Facet Size");
  facetSizeSpinBox.setValue(0.50);
  facetSizeSpinBox.setSingleStep(0.01);
  facetSizeLabel.setMaximumHeight(20);
  facetSizeSpinBox.setMaximumHeight(20);
  facetAngleSpinBox.setMaximumHeight(20);
  facetSizeSpinBox.setMaximumHeight(20);
  facetAngleLayout.addWidget(&facetAngleLabel);
  facetAngleLayout.addWidget(&facetAngleSpinBox);
  facetAngleLayout.addWidget(&facetSizeLabel);
  facetAngleLayout.addWidget(&facetSizeSpinBox);
  facetDistanceLabel.setText("Facet Distance");
  cellRadiusToEdgeRatioLabel.setText("Cell2Edge Ratio");
  facetDistanceLabel.setMaximumHeight(20);
  cellRadiusToEdgeRatioLabel.setMaximumHeight(20);
  facetDistanceSpinBox.setValue(0.03);
  facetDistanceSpinBox.setSingleStep(0.01);
  facetDistanceSpinBox.setMaximumHeight(20);
  cellRadiusToEdgeRatioSpinBox.setValue(3.0);
  cellRadiusToEdgeRatioSpinBox.setSingleStep(0.01);
  cellRadiusToEdgeRatioSpinBox.setMaximumHeight(20);
  facetDistanceLayout.addWidget(&facetDistanceLabel);
  facetDistanceLayout.addWidget(&facetDistanceSpinBox);
  facetDistanceLayout.addWidget(&cellRadiusToEdgeRatioLabel);
  facetDistanceLayout.addWidget(&cellRadiusToEdgeRatioSpinBox);
  generateTetrahedraButton.setText("Generate Tetrahedra");
  layout.addWidget(&labelTitle);
  layout.addLayout(&cellSizeLayout);
  layout.addLayout(&facetAngleLayout);
  layout.addLayout(&facetDistanceLayout);
  layout.addWidget(&generateTetrahedraButton);
}

QVariantMap CGALTetrahedralizeWidget::parameters() const {
  QVariantMap p;
  p["cellSize"] = cellSizeSpinBox.value();
  p["facetAngle"] = facetAngleSpinBox.value();
  p["facetSize"] = facetSizeSpinBox.value();
  p["facetDistance"] = facetDistanceSpinBox.value();
  p["cellRadiusEdgeRatio"] = cellRadiusToEdgeRatioSpinBox.value();
  return p;
}
