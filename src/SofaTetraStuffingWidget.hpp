/*
 * SofaTetraStuffingWidget.hpp
 *
 * This widget provides options for the SOFA TetraStuffing implementation.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */
#ifndef SOFA_TETRA_STUFFING_WIDGET_HPP
#define SOFA_TETRA_STUFFING_WIDGET_HPP

#include <BaseOptionsWidget.hpp>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <qgltetraviewer.h>

class SofaTetraStuffingWidget : public BaseOptionsWidget
{
  Q_OBJECT

public:
  SofaTetraStuffingWidget(QWidget *parent);
  void update();

  void generateTetrahedra(QGLTetraViewer *viewer_);

  QHBoxLayout tetraSizeLayout;
  QLabel tetraSizeLabel;
  QDoubleSpinBox tetraSizeSpinBox;
  QHBoxLayout alphaOptionsLayout;
  QLabel alphaShortLabel;
  QDoubleSpinBox alphaShortSpinBox;
  QLabel alphaLongLabel;
  QDoubleSpinBox alphaLongSpinBox;
  QCheckBox splitTetrasCheckBox;
  QCheckBox snapToPointsCheckBox;
  QPushButton generateTetrahedraButton;
  QHBoxLayout additionalOptionsLayout;
};

#endif // SOFA_TETRA_STUFFING_WIDGET_HPP
