/*
 * surfaceVisWidget.cpp
 *
 * This widget provides options for the surface visualization.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include <surfaceVisWidget.hpp>
#include <QFont>

SurfaceVisWidget::SurfaceVisWidget(QWidget *parent) : BaseOptionsWidget(parent)
{
  labelTitle.setText("Surface Visualization");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);
  surfaceColorButton.setText("Surface Color");
  surfaceWireframeColorButton.setText("Wireframe Color");
  surfaceVisComboBox.addItem("Surface Solid");
  surfaceVisComboBox.addItem("Surface Wireframe");
  surfaceVisComboBox.addItem("Surface Hidden");
  drawEdgesCheckbox.setText("Draw Edges?");
  edgeColorButton.setText("Edge Color");
  layout.addWidget(&labelTitle);
  layout.addWidget(&surfaceVisComboBox);
  layout.addWidget(&surfaceColorButton);
  layout.addWidget(&surfaceWireframeColorButton);
  layout.addWidget(&drawEdgesCheckbox);
  layout.addWidget(&edgeColorButton);
}
