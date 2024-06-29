/*
 * surfaceVisWidget.hpp
 *
 * This widget provides the UI options for the tetrahedra visualization.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#ifndef TETRA_VIS_WIDGET_H
#define TETRA_VIS_WIDGET_H

#include <BaseOptionsWidget.hpp>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <RangeSlider.hpp>

class TetraVisWidget : public BaseOptionsWidget {
  Q_OBJECT

 public:
  TetraVisWidget(QWidget *parent);
  void update();

  QComboBox tetraVisComboBox;
  QPushButton tetraColorButton;
  QPushButton tetraWireframeColorButton;
  QLabel tetraXCutplaneLabel;
  RangeSlider tetraXCutplaneSlider;
  QLabel tetraYCutplaneLabel;
  RangeSlider tetraYCutplaneSlider;
  QLabel tetraZCutplaneLabel;
  RangeSlider tetraZCutplaneSlider;
};

#endif  // TETRA_VIS_WIDGET_H
