/*
 * QuartetTetraStuffingWidget.hpp
 *
 * This widget provides options for Crawford Doran's implementation Quartet
 * TetraStuffing implementation.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */
#ifndef QUARTET_TETRA_STUFFING_WIDGET_HPP
#define QUARTET_TETRA_STUFFING_WIDGET_HPP

#include <QuartetTetraStuffing.h>
#include <qgltetraviewer.hpp>

#include <BaseOptionsWidget.hpp>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QPushButton>

class QuartetTetraStuffingWidget : public BaseOptionsWidget {
  Q_OBJECT

 public:
  QuartetTetraStuffingWidget(QWidget *parent);
  void update();

  void generateTetrahedra(QGLTetraViewer *viewer_);

  QHBoxLayout tetraSizeLayout;
  QLabel cellSizeLabel;
  QDoubleSpinBox cellSizeSpinBox;
  QHBoxLayout angleThresholdLayout;
  QLabel angleThresholdLabel;
  QDoubleSpinBox angleThresholdSpinBox;
  QPushButton generateTetrahedraButton;
};

#endif  // QUARTET_TETRA_STUFFING_WIDGET_HPP
