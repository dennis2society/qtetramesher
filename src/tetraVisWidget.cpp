/*
 * surfaceVisWidget.cpp
 *
 * This widget provides options for the surface visualization.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include <tetraVisWidget.hpp>
#include <QFont>

TetraVisWidget::TetraVisWidget(QWidget *parent) : BaseOptionsWidget(parent)
{
  labelTitle.setText("TetraMesh Visualization");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);
  tetraColorButton.setText("Tetrahedra Color");
  tetraWireframeColorButton.setText("Wireframe Color");
  tetraVisComboBox.addItem("Solid");
  tetraVisComboBox.addItem("Wireframe");
  tetraVisComboBox.addItem("Hidden");
  layout.addWidget(&labelTitle);
  layout.addWidget(&tetraVisComboBox);
  layout.addWidget(&tetraColorButton);
  layout.addWidget(&tetraWireframeColorButton);
}
