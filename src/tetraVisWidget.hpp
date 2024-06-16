/*
 * surfaceVisWidget.hpp
 *
 * This widget provides options for the surface visualization.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#ifndef TETRA_VIS_WIDGET_H
#define TETRA_VIS_WIDGET_H

#include <baseOptionsWidget.hpp>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>

class TetraVisWidget : public BaseOptionsWidget
{
  Q_OBJECT

public:
  TetraVisWidget(QWidget *parent);
  void update();

  QComboBox tetraVisComboBox;
  QPushButton tetraColorButton;
  QPushButton tetraWireframeColorButton;
};

#endif //TETRA_VIS_WIDGET_H
