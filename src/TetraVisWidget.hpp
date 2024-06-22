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
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QSlider>

class TetraVisWidget : public BaseOptionsWidget
{
  Q_OBJECT

public:
  TetraVisWidget(QWidget *parent);
  void update();

  QComboBox tetraVisComboBox;
  QPushButton tetraColorButton;
  QPushButton tetraWireframeColorButton;
  QLabel tetraCutplaneLabel;
  QSlider tetraCutplaneSlider;
};

#endif //TETRA_VIS_WIDGET_H
