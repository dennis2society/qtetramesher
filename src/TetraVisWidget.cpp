/*
 * surfaceVisWidget.cpp
 *
 * This widget provides options for the surface visualization.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include <QFont>
#include <TetraVisWidget.hpp>

TetraVisWidget::TetraVisWidget(QWidget *parent)
    : BaseOptionsWidget(parent),
      tetraZCutplaneSlider(Qt::Horizontal, RangeSlider::Option::DoubleHandles,
                           this) {
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
  tetraZCutplaneLabel.setText("Cutplanes Z");
  tetraZCutplaneLabel.setMaximumHeight(20);
  tetraZCutplaneSlider.SetLowerValue(0);
  tetraZCutplaneSlider.SetUpperValue(100);
  tetraZCutplaneSlider.SetRange(0, 100);
  tetraZCutplaneSlider.setSizePolicy(QSizePolicy::Preferred,
                                     QSizePolicy::Preferred);
  // tetraCutplaneSlider.setSingleStep(1);
  tetraZCutplaneSlider.setMinimumSize(280, 20);
  tetraZCutplaneSlider.setMaximumHeight(20);
  tetraZCutplaneSlider.setMaximumWidth(320);
  layout.addWidget(&labelTitle);
  layout.addWidget(&tetraVisComboBox);
  layout.addWidget(&tetraColorButton);
  layout.addWidget(&tetraWireframeColorButton);
  layout.addWidget(&tetraZCutplaneLabel);
  layout.addWidget(&tetraZCutplaneSlider);
  layout.addStretch();
}
