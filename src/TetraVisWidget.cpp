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
      tetraXCutplaneSlider(Qt::Horizontal, RangeSlider::Option::DoubleHandles,
                           this),
      tetraYCutplaneSlider(Qt::Horizontal, RangeSlider::Option::DoubleHandles,
                           this),
      tetraZCutplaneSlider(Qt::Horizontal, RangeSlider::Option::DoubleHandles,
                           this) {
  labelTitle.setText("TetraMesh Visualization");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);
  drawWireframeCheckbox.setText("Draw Wireframe");
  drawWireframeCheckbox.setChecked(true);
  drawWireframeCheckbox.setMaximumHeight(20);
  drawSolidCheckbox.setText("Draw Solid");
  drawSolidCheckbox.setChecked(true);
  drawSolidCheckbox.setMaximumHeight(20);
  tetraColorButton.setText("Tetrahedra Color");
  tetraColorButton.setMaximumHeight(20);
  tetraWireframeColorButton.setText("Wireframe Color");
  tetraWireframeColorButton.setMaximumHeight(20);
  //  X cutplanes
  tetraXCutplaneLabel.setText("Cutplanes X");
  tetraXCutplaneLabel.setMaximumHeight(20);
  tetraXCutplaneSlider.SetLowerValue(1);
  tetraXCutplaneSlider.SetUpperValue(100);
  tetraXCutplaneSlider.SetRange(1, 100);
  tetraXCutplaneSlider.setSizePolicy(QSizePolicy::Preferred,
                                     QSizePolicy::Preferred);
  tetraXCutplaneSlider.setMinimumSize(280, 20);
  tetraXCutplaneSlider.setMaximumHeight(20);
  tetraXCutplaneSlider.setMaximumWidth(320);
  // Y cutplanes
  tetraYCutplaneLabel.setText("Cutplanes Y");
  tetraYCutplaneLabel.setMaximumHeight(20);
  tetraYCutplaneSlider.SetLowerValue(1);
  tetraYCutplaneSlider.SetUpperValue(100);
  tetraYCutplaneSlider.SetRange(1, 100);
  tetraYCutplaneSlider.setSizePolicy(QSizePolicy::Preferred,
                                     QSizePolicy::Preferred);
  tetraYCutplaneSlider.setMinimumSize(280, 20);
  tetraYCutplaneSlider.setMaximumHeight(20);
  tetraYCutplaneSlider.setMaximumWidth(320);
  // Z cutplanes
  tetraZCutplaneLabel.setText("Cutplanes Z");
  tetraZCutplaneLabel.setMaximumHeight(20);
  tetraZCutplaneSlider.SetLowerValue(1);
  tetraZCutplaneSlider.SetUpperValue(100);
  tetraZCutplaneSlider.SetRange(1, 100);
  tetraZCutplaneSlider.setSizePolicy(QSizePolicy::Preferred,
                                     QSizePolicy::Preferred);
  tetraZCutplaneSlider.setMinimumSize(280, 20);
  tetraZCutplaneSlider.setMaximumHeight(20);
  tetraZCutplaneSlider.setMaximumWidth(320);
  // add everything to the layout
  layout.addWidget(&labelTitle);
  // layout.addWidget(&tetraVisComboBox);
  drawOptionsLayout.addWidget(&drawWireframeCheckbox);
  drawOptionsLayout.addWidget(&drawSolidCheckbox);
  layout.addLayout(&drawOptionsLayout);
  layout.addWidget(&tetraColorButton);
  layout.addWidget(&tetraWireframeColorButton);
  layout.addWidget(&tetraXCutplaneLabel);
  layout.addWidget(&tetraXCutplaneSlider);
  layout.addWidget(&tetraYCutplaneLabel);
  layout.addWidget(&tetraYCutplaneSlider);
  layout.addWidget(&tetraZCutplaneLabel);
  layout.addWidget(&tetraZCutplaneSlider);
  layout.addStretch();
}
