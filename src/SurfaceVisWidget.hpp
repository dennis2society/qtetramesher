/*
 * surfaceVisWidget.hpp
 *
 * This widget provides the UI options for the surface visualization.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#ifndef SURFACE_VIS_WIDGET_H
#define SURFACE_VIS_WIDGET_H

#include <BaseOptionsWidget.hpp>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>

class SurfaceVisWidget : public BaseOptionsWidget
{
  Q_OBJECT

public:
  SurfaceVisWidget(QWidget *parent);
  void update();

  QComboBox surfaceVisComboBox;
  QPushButton surfaceColorButton;
  QPushButton surfaceWireframeColorButton;
};

#endif //SURFACE_VIS_WIDGET_H
