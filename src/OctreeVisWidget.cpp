/*
 * surfaceVisWidget.cpp
 *
 * This widget provides the UI options for the octree visualization.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include "OctreeVisWidget.hpp"
#include <QFont>

OctreeVisWidget::OctreeVisWidget(QWidget *parent) : BaseOptionsWidget(parent)
{
  labelTitle.setText("Octree Visualization");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);
  octreeVisComboBox.addItem("Hidden");
  octreeVisComboBox.addItem("Octree");
  octreeVisComboBox.addItem("leaves Only");
  octreeVisComboBox.setMaximumHeight(20);
  octreeGenerateButton.setText("Generate Octree");
  octreeGenerateButton.setToolTip("Generate the octree with the selected depth");
  octreeGenerateButton.setMaximumHeight(20);
  octreeDepthSpinBox.setRange(1, 7);
  octreeDepthSpinBox.setValue(3);
  octreeDepthSpinBox.setMaximumHeight(20);
  octreeVisOptionsLabel.setText("Octree Options");
  octreeVisOptionsLabel.setFont(boldFont);
  octreeVisOptionsLabel.setMaximumHeight(20);
  innerLayout.addWidget(&octreeDepthSpinBox);
  innerLayout.addWidget(&octreeGenerateButton);
  layout.addWidget(&labelTitle);
  layout.addWidget(&octreeVisComboBox);
  layout.addWidget(&octreeVisOptionsLabel);
  layout.addLayout(&innerLayout);
  //layout.addWidget(&surfaceVisComboBox);
  //layout.addWidget(&surfaceColorButton);
  //layout.addWidget(&surfaceWireframeColorButton);
  this->setLayout(&layout);
  layout.addStretch();
}
