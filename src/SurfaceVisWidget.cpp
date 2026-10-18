/*
 * surfaceVisWidget.cpp
 *
 * This widget provides options for the surface visualization.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include <QFont>
#include <SurfaceVisWidget.hpp>

SurfaceVisWidget::SurfaceVisWidget(QWidget *parent)
    : BaseOptionsWidget(parent) {
  labelTitle.setText("Surface Visualization");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);
  surfaceColorButton.setText("Surface Color");
  surfaceColorButton.setMaximumHeight(20);
  surfaceWireframeColorButton.setText("Wireframe Color");
  surfaceWireframeColorButton.setMaximumHeight(20);
  surfaceVisComboBox.addItem("Solid");
  surfaceVisComboBox.addItem("Wireframe");
  surfaceVisComboBox.addItem("Hidden");
  surfaceVisComboBox.setMaximumHeight(20);
  checkSurfaceButton.setText("Check Surface Mesh");
  checkSurfaceButton.setToolTip(
      "Check for intersecting triangles, open edges and other defects that "
      "break Tetgen/GMSH");
  checkSurfaceButton.setMaximumHeight(20);
  layout.addWidget(&labelTitle);
  layout.addWidget(&surfaceVisComboBox);
  layout.addWidget(&surfaceColorButton);
  layout.addWidget(&surfaceWireframeColorButton);
  layout.addWidget(&checkSurfaceButton);
  layout.addStretch();
}
