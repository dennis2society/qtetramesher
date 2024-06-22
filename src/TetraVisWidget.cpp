/*
 * surfaceVisWidget.cpp
 *
 * This widget provides options for the surface visualization.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include <TetraVisWidget.hpp>
#include <QFont>

TetraVisWidget::TetraVisWidget(QWidget *parent) : BaseOptionsWidget(parent)
{
  labelTitle.setText("TetraMesh Visualization");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);
  tetraColorButton.setText("Tetrahedra Color");
  tetraColorButton.setMaximumHeight(20);
  tetraWireframeColorButton.setText("Wireframe Color");
  tetraWireframeColorButton.setMaximumHeight(20);
  tetraVisComboBox.addItem("Solid");
  tetraVisComboBox.addItem("Wireframe");
  tetraVisComboBox.addItem("Hidden");
  tetraVisComboBox.setMaximumHeight(20);
  tetraCutplaneLabel.setText("Cutplane Z");
  tetraCutplaneLabel.setMaximumHeight(20);
  tetraCutplaneSlider.setValue(0);
  tetraCutplaneSlider.setRange(0, 100);
  tetraCutplaneSlider.setOrientation(Qt::Orientation::Horizontal);
  tetraCutplaneSlider.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  tetraCutplaneSlider.setSingleStep(1);
  tetraCutplaneSlider.setMinimumSize(280, 20);
  tetraCutplaneSlider.setMaximumHeight(20);
  tetraCutplaneSlider.setMaximumWidth(320);
  layout.addWidget(&labelTitle);
  layout.addWidget(&tetraVisComboBox);
  layout.addWidget(&tetraColorButton);
  layout.addWidget(&tetraWireframeColorButton);
  layout.addWidget(&tetraCutplaneLabel);
  layout.addWidget(&tetraCutplaneSlider);
  this->setLayout(&layout);
  layout.addStretch();
}
